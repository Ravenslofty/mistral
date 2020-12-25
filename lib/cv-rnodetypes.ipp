P(NONE),   // Invalid route node

// Clock networks
P(GCLK),    // Global clock, rooted at the clockbuf in the middle of the horizontal and vertical borders
P(RCLK),    // Regional clock (quarter of the die), rooted to the three corners of the quarters on the border
P(PCLK),    // Peripheral clock, driven from a HSSI or a DCMUX on the left column
P(SCLK),    // Sector clock, spans a rectangular zone, connects from GCLK/RCLK/PCLK
P(BCLK),    // Border clock, rooted at regular distance on the top and bottom lines, connects from SCLK
P(HCLK),    // Horizontal clock, spans 10-15 tiles horizontally, connects from SCLK
P(TCLK),    // Tile clock, connects from HCLK or BCLK
P(PMUX),    // Selects between multiple SCLK at the input of a PLL

// PLL feedback clock networks
P(GCLKFB),  // Feedback from a global clock mux
P(RCLKFB),  // Feedback from a regional clock mux

// Generic data input/output
P(GIN),
P(GOUT),

// Global data routing
P(H14),
P(H3),
P(H6),
P(V12),
P(V2),
P(V4),
P(WM),

// In-tile routing
P(TD),      // Tile data dispatch, connects from V*, H*, BCLK, HCLK and GIN
P(LD),      // Loopback dispatch, connects from some outputs of a LAB to some of its inputs
P(TDMUX),   // Mux over a set of TD to select what to give a LCMUX
P(DCMUX),   // 2-port mux selecting between a TCLK and a TDMUX for a clock-like input
