library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity adder is
    Port ( A : in  STD_LOGIC_VECTOR (31 downto 0);
           B : in  STD_LOGIC_VECTOR (31 downto 0);
           O : out  STD_LOGIC_VECTOR (31 downto 0));
end adder;

architecture Behavioral of adder is
    signal tmp : unsigned(31 downto 0);
begin

    tmp <= unsigned(A) + unsigned(B);
    O <= std_logic_vector(tmp);

end Behavioral;

