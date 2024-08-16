library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity splitter is
  port (
    irq: in std_logic_vector(4 downto 0); -- Assuming irq is a 5-bit vector representing interrupts
    irq2, irq3, irq4, irq5, irq6: out std_logic
  );
end entity splitter;

architecture behavioral of splitter is
  attribute X_INTERFACE_INFO : STRING;
  attribute X_INTERFACE_INFO of irq2 : SIGNAL is "xilinx.com:signal:interrupt:1.0 irq2 INTERRUPT"; -- use this
  attribute X_INTERFACE_INFO of irq3 : SIGNAL is "xilinx.com:signal:interrupt:1.0 irq3 INTERRUPT";
  attribute X_INTERFACE_INFO of irq4 : SIGNAL is "xilinx.com:signal:interrupt:1.0 irq4 INTERRUPT";
  attribute X_INTERFACE_INFO of irq5 : SIGNAL is "xilinx.com:signal:interrupt:1.0 irq5 INTERRUPT";
  attribute X_INTERFACE_INFO of irq6 : SIGNAL is "xilinx.com:signal:interrupt:1.0 irq6 INTERRUPT";
begin
  process(irq)
  begin
    irq2 <= irq(0);
    irq3 <= irq(1);
    irq4 <= irq(2);
    irq5 <= irq(3);
    irq6 <= irq(4);
  end process;
end architecture behavioral;