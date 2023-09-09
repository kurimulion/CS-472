library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

library work;
use work.array_t.all;

entity fir_base is
    Port ( 
        Reset : in  STD_LOGIC;
        Clk : in  STD_LOGIC;
        Input : in array32_t(0 to 9);
        Output : out  STD_LOGIC_VECTOR (31 downto 0)
    );
end fir_base;

architecture Behavioral of fir_base is
    
    component adder 
    Port ( A : in  STD_LOGIC_VECTOR (31 downto 0);
           B : in  STD_LOGIC_VECTOR (31 downto 0);
           O : out  STD_LOGIC_VECTOR (31 downto 0));
    end component;
    
    component mul32_32 
    Port ( A : in  STD_LOGIC_VECTOR (31 downto 0);
           B : in  STD_LOGIC_VECTOR (31 downto 0);
           O : out  STD_LOGIC_VECTOR (31 downto 0));
    end component;
    
    signal mout : array32_t(0 to 9);
    signal aout : array32_t(0 to 8);
    
begin

    f1: for i in 0 to 9 generate
        mul: mul32_32 port map (
            A => Input(i),
            B => fir_const(i),
            O => mout(i)
        );
    end generate f1;
    
    f2: for i in 0 to 8 generate
        if1: if i = 0 generate
            add0: adder port map (
                A => mout(i),
                B => mout(i + 1),
                O => aout(i)
            );
        end generate if1;
        if2: if i > 0 generate
            add: adder port map (
                A => aout(i - 1),
                B => mout(i + 1),
                O => aout(i)
            );
        end generate if2;
    end generate f2;  
    
    Output <= aout(8);
    
end Behavioral;

