module compute(clock ,reset, display_col, display_row, color);

    input clock, reset;

    output wire [23:0] color;

    input [11:0] display_col; // column number of pixel on the screen
    input [10:0] display_row; // row number of pixel on the screen

    reg [10:0] board_read_address;
    wire [4:0] board_read_data;

    reg [12:0] sprite_read_address;
    wire [5:0] sprite_read_data;

    board b(
        .clock(clock),
        .rdaddress(board_read_address),
        .q(board_read_data)
    );

    sprites s(
        .clock(clock),
        .rdaddress(sprite_read_address),
        .q(sprite_read_data)
    );

    colors c(
        .clock(clock),
        .rdaddress(sprite_read_data),
        .q(color)
    );
     
    always @(posedge clock) board_read_address = display_row[10:5] * 40 + display_col[11:5];
    always @(posedge clock) sprite_read_address = board_read_data * 256 + display_row[4:1] * 16 + display_col[4:1];

endmodule
