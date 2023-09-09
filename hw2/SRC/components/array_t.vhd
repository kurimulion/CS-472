library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

PACKAGE array_t IS
	TYPE array32_t IS array (natural range <>) of std_logic_vector(31 downto 0);
	TYPE array_int_t IS array (natural range <>) of integer;
	constant fir_const: array32_t(0 to 9) := ( --"b0~b9"
		x"00000013", -- b0 = 19
		x"00000186", -- b1 = 390
		x"00000360", -- b2 = 864
		x"00000C00", -- b3 = 3072
		x"00000009", -- b4 = 9
		x"00000009", -- b5 = 9
		x"00000C00", -- b6 = 3072
		x"00000360", -- b7 = 864
		x"00000186", -- b8 = 390
		x"00000013"  -- b9 = 19
	);
END array_t;
