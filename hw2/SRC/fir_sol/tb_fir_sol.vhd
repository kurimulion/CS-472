LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.std_logic_unsigned.all;
USE ieee.numeric_std.ALL;

library work;
use work.array_t.all;

ENTITY tb_fir_sol IS
END tb_fir_sol;

ARCHITECTURE testbench OF tb_fir_sol IS 

    -- Component Declaration for the Unit Under Test (UUT)
    COMPONENT fir_sol_wrapper
    PORT(
        Reset : in  STD_LOGIC;
        Clk : in  STD_LOGIC;
        Input : in  STD_LOGIC_VECTOR (31 downto 0);
        Output : out  STD_LOGIC_VECTOR (31 downto 0)
        );
    END COMPONENT;

    COMPONENT fir_base_wrapper
    PORT(
        Reset : in  STD_LOGIC;
        Clk : in  STD_LOGIC;
        Input : in  STD_LOGIC_VECTOR (31 downto 0);
        Output : out  STD_LOGIC_VECTOR (31 downto 0)
        );
    END COMPONENT;

    --Inputs
    SIGNAL Reset :  std_logic := '1';
    SIGNAL Clk :  std_logic := '0';
    SIGNAL I :  std_logic_vector(31 downto 0) := (others=>'0');

    --Outputs
    SIGNAL Osol:  std_logic_vector(31 downto 0);
    
    constant sin : array_int_t(0 to 19) := (
        1024,
        1340,
        1626,
        1852,
        1998,
        2048,
        1998,
        1852,
        1625,
        1340,
        1024,
        708,
        422,
        196,
        50,
        0,
        50,
        195,
        422,
        707
    );
    
BEGIN

  Reset <= '0' after 100 ns;

    process begin
        Clk <= '1'; wait for 5 ns;
        Clk <= '0'; wait for 5 ns;
    end process;
    
    -- Instantiate the Unit Under Test (UUT)
    uut1: fir_sol_wrapper PORT MAP(
        Reset => Reset,
        Clk => Clk,
        Input => I,
        Output => Osol
    );

    tb : PROCESS(Clk)
        variable it : integer := 0;
    BEGIN
        if Clk'event and Clk = '1' then
            I <= std_logic_vector(to_unsigned(sin(it), 32));
            it := it + 1;
            if it = 20 then
                it := 0;
            end if;
        end if;
    END PROCESS;

END testbench;
