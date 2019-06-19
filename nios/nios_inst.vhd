	component nios is
		port (
			buttons_export      : in  std_logic_vector(3 downto 0)  := (others => 'X'); -- export
			clk_clk             : in  std_logic                     := 'X';             -- clk
			data_export         : out std_logic_vector(31 downto 0);                    -- export
			leds_export         : out std_logic_vector(7 downto 0);                     -- export
			operation_export    : out std_logic_vector(31 downto 0);                    -- export
			resetn_reset_n      : in  std_logic                     := 'X';             -- reset_n
			switches_export     : in  std_logic_vector(7 downto 0)  := (others => 'X'); -- export
			uart_0_external_rxd : in  std_logic                     := 'X';             -- rxd
			uart_0_external_txd : out std_logic                                         -- txd
		);
	end component nios;

	u0 : component nios
		port map (
			buttons_export      => CONNECTED_TO_buttons_export,      --         buttons.export
			clk_clk             => CONNECTED_TO_clk_clk,             --             clk.clk
			data_export         => CONNECTED_TO_data_export,         --            data.export
			leds_export         => CONNECTED_TO_leds_export,         --            leds.export
			operation_export    => CONNECTED_TO_operation_export,    --       operation.export
			resetn_reset_n      => CONNECTED_TO_resetn_reset_n,      --          resetn.reset_n
			switches_export     => CONNECTED_TO_switches_export,     --        switches.export
			uart_0_external_rxd => CONNECTED_TO_uart_0_external_rxd, -- uart_0_external.rxd
			uart_0_external_txd => CONNECTED_TO_uart_0_external_txd  --                .txd
		);

