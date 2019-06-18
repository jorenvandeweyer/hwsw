	component unsaved is
		port (
			clk_clk             : in  std_logic := 'X'; -- clk
			resetn_reset_n      : in  std_logic := 'X'; -- reset_n
			uart_0_external_rxd : in  std_logic := 'X'; -- rxd
			uart_0_external_txd : out std_logic         -- txd
		);
	end component unsaved;

	u0 : component unsaved
		port map (
			clk_clk             => CONNECTED_TO_clk_clk,             --             clk.clk
			resetn_reset_n      => CONNECTED_TO_resetn_reset_n,      --          resetn.reset_n
			uart_0_external_rxd => CONNECTED_TO_uart_0_external_rxd, -- uart_0_external.rxd
			uart_0_external_txd => CONNECTED_TO_uart_0_external_txd  --                .txd
		);

