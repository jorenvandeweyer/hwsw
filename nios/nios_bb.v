
module nios (
	clk_clk,
	leds_export,
	resetn_reset_n,
	switches_export,
	uart_0_external_rxd,
	uart_0_external_txd,
	buttons_export);	

	input		clk_clk;
	output	[7:0]	leds_export;
	input		resetn_reset_n;
	input	[7:0]	switches_export;
	input		uart_0_external_rxd;
	output		uart_0_external_txd;
	input	[3:0]	buttons_export;
endmodule
