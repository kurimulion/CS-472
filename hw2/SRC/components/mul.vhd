library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity mul32_32 is
    Port ( A : in  STD_LOGIC_VECTOR (31 downto 0);
           B : in  STD_LOGIC_VECTOR (31 downto 0);
           O : out  STD_LOGIC_VECTOR (31 downto 0));
end mul32_32;

architecture Behavioral of mul32_32 is
    signal tmp : unsigned(63 downto 0);
begin

    tmp <= unsigned(A) * unsigned(B);
    O <= std_logic_vector(tmp(31 downto 0));
    
end Behavioral;

