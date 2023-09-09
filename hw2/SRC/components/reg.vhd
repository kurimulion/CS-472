library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity reg is
    Port ( Reset : in  STD_LOGIC;
           Clk : in  STD_LOGIC;
           Load : in  STD_LOGIC;
           Din : in  STD_LOGIC_VECTOR (31 downto 0);
           Dout : out  STD_LOGIC_VECTOR (31 downto 0));
end reg;

architecture Behavioral of reg is

begin
    
    process(Clk)
    begin
        if Clk = '1' and Clk'Event then
            if Reset = '1' then
                Dout <= (others => '0');
            elsif Load = '1' then
                Dout <= Din;
            end if;
        end if;
    end process;

end Behavioral;

