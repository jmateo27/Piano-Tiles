----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 02/24/2024 08:19:53 PM
-- Design Name: 
-- Module Name: tile_generator - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity tile_generator is
    GENERIC (
        DATA_LEN : INTEGER := 32;
        COLUMN_LEN : INTEGER := 10
    );
    PORT (
        read: in STD_LOGIC;
        axi_resetn : in STD_LOGIC;
        aclk : in STD_LOGIC;
        S_AXIS_TDATA : in STD_LOGIC_VECTOR (DATA_LEN-1 downto 0);
        S_AXIS_TLAST : in STD_LOGIC;
        S_AXIS_TVALID : in STD_LOGIC;
        S_AXIS_TREADY : out STD_LOGIC;
        M_AXIS_TDATA : out STD_LOGIC_VECTOR (DATA_LEN-1 downto 0);
        M_AXIS_TVALID : out STD_LOGIC;
        M_AXIS_TLAST : out STD_LOGIC
    );
end tile_generator;

architecture Behavioural of tile_generator is

    type state is (init, start, reads, square, refresh, fin);
    signal n_state : state;
    signal c_state : state := init;
    signal op : UNSIGNED(DATA_LEN downto 0) := (others => '0');

    signal count_out : INTEGER := 0;
    signal max_out : unsigned(DATA_LEN-1 downto 0);
    signal re_out, im_out : unsigned(DATA_LEN downto 0);
    
    signal magnitude_in : STD_LOGIC_VECTOR (DATA_LEN-1 downto 0);
    signal input_valid, last, clk, rst : STD_LOGIC;
    signal ready : STD_LOGIC;
    signal magnitude_out : STD_LOGIC_VECTOR (DATA_LEN-1 downto 0);
    signal column_out : STD_LOGIC_VECTOR (COLUMN_LEN-1 downto 0);
    signal done : STD_LOGIC;
    
begin
    
    clk <= aclk;
    rst <= axi_resetn;
    
    magnitude_in <= S_AXIS_TDATA;
    last <= S_AXIS_TLAST;
    input_valid <= S_AXIS_TVALID;
    S_AXIS_TREADY <= ready;
    
    M_AXIS_TDATA <= column_out & magnitude_out(DATA_LEN/2-1 + 6 downto 0);
    M_AXIS_TVALID <= done;
        
    process(c_state, input_valid, read)
        variable count : INTEGER := 0;
        variable max : unsigned(DATA_LEN-1 downto 0);
        variable re, im : unsigned(DATA_LEN downto 0);
    begin
        case c_state is 
            when init =>
                max := to_unsigned(0, max'length);
                column_out <= (others => '0');
                done <= '0';
                ready <= '0';
                count := 0;
                n_state <= start;
                M_AXIS_TLAST <= '0';
                
            when start =>
                ready <= '1';
                if input_valid = '1' then
                    n_state <= reads;
                end if;
                
            when reads =>
                ready <= '0';
                re := to_unsigned(0, DATA_LEN/2+1) & unsigned(magnitude_in(DATA_LEN-1 downto DATA_LEN/2));
                im := to_unsigned(0, DATA_LEN/2+1) & unsigned(magnitude_in(DATA_LEN/2-1 downto 0));
                n_state <= square;
            
            when square =>
                re(DATA_LEN-1 downto 0) := re(DATA_LEN/2-1 downto 0) * re(DATA_LEN/2-1 downto 0);
                im(DATA_LEN-1 downto 0) := im(DATA_LEN/2-1 downto 0) * im(DATA_LEN/2-1 downto 0);
                op <= re + im;
                n_state <= refresh;
            
            when refresh =>
                if last = '1' then
                    count := 1023;
                end if;

                if max < op then
                    if op(DATA_LEN) = '1' then
                        op <= to_unsigned(0, op'length) - 1; -- overflow -> max value
                    end if;
                    max := op(DATA_LEN-1 downto 0);
                    magnitude_out <= std_logic_vector(max);
                    column_out <= std_logic_vector(to_unsigned(count, column_out'length));
                end if;
                
                count := count + 1;

                if count = 1024 then
                    n_state <= fin;
                    done <= '1';
                    magnitude_out <= std_logic_vector(max);
                    count := 0;
                else
                    n_state <= start;
                end if;
            
            when fin =>
                done <= '1';
                magnitude_out <= std_logic_vector(max);
                M_AXIS_TLAST <= '1';
                if read = '1' then
                    n_state <= init;
                end if;
            
            when others =>
                n_state <= init;
            
        end case;
        count_out <= count;
        max_out <= max;
        re_out <= re;
        im_out <= im;
    
    end process;
    
    process(clk, rst, last)
    begin
        if rst = '0' then
            c_state <= init;
        elsif last = '1' then
            c_state <= refresh;
        elsif rising_edge(clk) then
            c_state <= n_state;
        end if;
    end process;
 
end Behavioural;
