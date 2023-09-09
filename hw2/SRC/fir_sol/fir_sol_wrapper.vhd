library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

library work;
use work.array_t.all;

entity fir_sol_wrapper is
    Port ( 
        Reset : in  STD_LOGIC;
        Clk : in  STD_LOGIC;
        Input : in  STD_LOGIC_VECTOR (31 downto 0);
        Output : out  STD_LOGIC_VECTOR (31 downto 0)
    );
end fir_sol_wrapper;

architecture Structural of fir_sol_wrapper is
    
    signal del : array32_t(0 to 9);
    signal outs : STD_LOGIC_VECTOR (31 downto 0);
    
    component reg 
    Port ( Reset : in  STD_LOGIC;
           Clk : in  STD_LOGIC;
           Load : in  STD_LOGIC;
           Din : in  STD_LOGIC_VECTOR (31 downto 0);
           Dout : out  STD_LOGIC_VECTOR (31 downto 0));
    end component;
    
    component fir_sol
    Port ( Reset : in  STD_LOGIC;
           Clk : in  STD_LOGIC;
           Input : in array32_t(0 to 9);
           Output : out  STD_LOGIC_VECTOR (31 downto 0));
    end component;
    
begin

    del(0) <= Input;
    f1: for i in 0 to 8 generate
        R: reg PORT MAP (
            Reset => Reset,
            Clk => Clk,
            Load => '1',
            Din => del(i),
            Dout => del(i+1)
        );
    end generate f1; 
    
    U1 : fir_sol Port Map (
        Reset => Reset,
        Clk => Clk,
        Input => del,
        Output => outs
    );
    
    R_o: reg PORT MAP (
        Reset => Reset,
        Clk => Clk,
        Load => '1',
        Din => outs,
        Dout => Output
    );
    
end Structural;

