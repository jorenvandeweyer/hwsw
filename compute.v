module compute(clock ,reset, display_col, display_row, color, operation, data);

    input clock, reset;

    output wire [23:0] color;

    input [11:0] display_col; // column number of pixel on the screen
    input [10:0] display_row; // row number of pixel on the screen

    input [31:0] operation, data;

    reg wren_board, wren_sprites, wren_colors;


    reg [10:0] board_read_address;
    wire [4:0] board_read_data;

    reg [12:0] sprite_read_address;
    wire [5:0] sprite_read_data;

    always @(posedge clock) begin
        if (reset) begin
            wren_board = 0;
            wren_sprites = 0;
            wren_colors = 0;
        end else begin
            case (operation[17:16])
                2'b01: begin
                    wren_board = 1;
                    wren_sprites = 0;
                    wren_colors = 0;
                end
                2'b10: begin
                    wren_board = 0;
                    wren_sprites = 1;
                    wren_colors = 0;
                end
                2'b11: begin
                    wren_board = 0;
                    wren_sprites = 0;
                    wren_colors = 1;
                end
                default: begin
                    wren_board = 0;
                    wren_sprites = 0;
                    wren_colors = 0;
                end
            endcase
        end
    end

    board b(
        .clock(clock),
        .data(data),
        .rdaddress(board_read_address),
        .wraddress(operation[10:0]),
        .wren(wren_board),
        .q(board_read_data)
    );

    sprites s(
        .clock(clock),
        .data(data),
        .rdaddress(sprite_read_address),
        .wraddress(operation[12:0]),
        .wren(wren_sprites),
        .q(sprite_read_data)
    );

    colors c(
        .clock(clock),
        .data(data),
        .rdaddress(sprite_read_data),
        .wraddress(operation[5:0]),
        .wren(wren_colors),
        .q(color)
    );
     
    always @(posedge clock) board_read_address = display_row[10:5] * 80 + display_col[11:5];
    always @(posedge clock) sprite_read_address = {{board_read_data}, {display_row[4:1]}, {display_col[4:1]}};

endmodule
