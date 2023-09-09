library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

library work;
use work.array_t.all;

entity fir_sol is
    Port ( 
        Reset : in  STD_LOGIC;
        Clk : in  STD_LOGIC;
        Input : in array32_t(0 to 9); -- This data type is defined in components/array_t.vhd
        Output : out  STD_LOGIC_VECTOR (31 downto 0)
  );
end fir_sol;

architecture Behavioral of fir_sol is
    -- These are the only components you can use. Note that you do not have to use them all.
    -- You can look up their implementation in components/*.vhd
    component adder 
    Port ( A : in  STD_LOGIC_VECTOR (31 downto 0);
           B : in  STD_LOGIC_VECTOR (31 downto 0);
           O : out  STD_LOGIC_VECTOR (31 downto 0));
    end component;

    component reg 
    Port ( Reset : in  STD_LOGIC;
           Clk : in  STD_LOGIC;
           Load : in  STD_LOGIC;
           Din : in  STD_LOGIC_VECTOR (31 downto 0);
           Dout : out  STD_LOGIC_VECTOR (31 downto 0));
    end component;

    component mul32_32 
    Port ( A : in  STD_LOGIC_VECTOR (31 downto 0);
           B : in  STD_LOGIC_VECTOR (31 downto 0);
           O : out  STD_LOGIC_VECTOR (31 downto 0));
    end component;

begin
    -- Your code should start here
    -- IMPORTANT: This is the only VHDL file you can change

end Behavioral;

