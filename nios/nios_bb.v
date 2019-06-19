
module nios (
	buttons_export,
	clk_clk,
	data_export,
	leds_export,
	operation_export,
	resetn_reset_n,
	switches_export,
	uart_0_external_rxd,
	uart_0_external_txd);	

	input	[3:0]	buttons_export;
	input		clk_clk;
	output	[31:0]	data_export;
	output	[7:0]	leds_export;
	output	[31:0]	operation_export;
	input		resetn_reset_n;
	input	[7:0]	switches_export;
	input		uart_0_external_rxd;
	output		uart_0_external_txd;
endmodule
