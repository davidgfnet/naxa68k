
// Definitions here for cleaniness reasons :)

const struct t_enemy_desc enemy_table[] = {
	{ 400,  42, ENEMY_ORANGE_PLANE | SHOOT_BLUE_WIDE | MOVE_IDENTITY, 0},
	{ 440, 106, ENEMY_ORANGE_PLANE | SHOOT_BLUE_WIDE | MOVE_IDENTITY, 0},
	{ 480, 170, ENEMY_ORANGE_PLANE | SHOOT_BLUE_WIDE | MOVE_IDENTITY, 0},
	{ 800,  74, ENEMY_PURPLE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 0},
	{ 840, 138, ENEMY_PURPLE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 0},
	{ 880, 202, ENEMY_PURPLE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 0},

	{1160,  50, ENEMY_ORANGE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 0},
	{1180, 100, ENEMY_ORANGE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 0},
	{1220,  50, ENEMY_ORANGE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 20},
	{1240, 100, ENEMY_ORANGE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 20},
	{1320, 160, ENEMY_PURPLE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 0},
	{1340, 210, ENEMY_PURPLE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 0},
	{1380, 160, ENEMY_PURPLE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 20},
	{1400, 210, ENEMY_PURPLE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 20},

	{1600, 110, ENEMY_ALIEN_SHIP | SHOOT_BLUE_NARR | MOVE_CIRCLE, 3},
	{1620, 160, ENEMY_ALIEN_SHIP | SHOOT_BLUE_NARR | MOVE_CIRCLE, 3},
	{1660, 130, ENEMY_ALIEN_SHIP | SHOOT_BLUE_NARR | MOVE_CIRCLE, 3},
	{1680, 180, ENEMY_ALIEN_SHIP | SHOOT_BLUE_NARR | MOVE_CIRCLE, 3},

	{1900, 100, ENEMY_BIG_JT | SHOOT_BLUE_WIDE | MOVE_CIRCLE, 3},
	{1980,  50, ENEMY_BIG_JT | SHOOT_BLUE_WIDE | MOVE_CIRCLE, 3},
	{1980, 150, ENEMY_BIG_JT | SHOOT_BLUE_WIDE | MOVE_CIRCLE, 3},

	{2160,  50, ENEMY_ORANGE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 0},
	{2180, 100, ENEMY_ORANGE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 0},
	{2220,  50, ENEMY_ORANGE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 20},
	{2240, 100, ENEMY_ORANGE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 20},
	{2320, 160, ENEMY_PURPLE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 0},
	{2340, 210, ENEMY_PURPLE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 0},
	{2380, 160, ENEMY_PURPLE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 20},
	{2400, 210, ENEMY_PURPLE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 20},
	{2460,  50, ENEMY_ORANGE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 0},
	{2480, 100, ENEMY_ORANGE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 0},
	{2520,  50, ENEMY_ORANGE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 20},
	{2540, 100, ENEMY_ORANGE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 20},
	{2620, 160, ENEMY_PURPLE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 0},
	{2640, 210, ENEMY_PURPLE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 0},
	{2680, 160, ENEMY_PURPLE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 20},
	{2700, 210, ENEMY_PURPLE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 20},

	{2720,  50, ENEMY_ORANGE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 0},
	{2740, 100, ENEMY_ORANGE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 0},
	{2760, 150, ENEMY_ORANGE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 20},
	{2780, 200, ENEMY_ORANGE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 20},
	{2820,  50, ENEMY_PURPLE_PLANE | SHOOT_ORAN_WIDE | MOVE_SWING, 0},
	{2840, 100, ENEMY_PURPLE_PLANE | SHOOT_ORAN_WIDE | MOVE_SWING, 0},
	{2860, 150, ENEMY_PURPLE_PLANE | SHOOT_ORAN_WIDE | MOVE_SWING, 20},
	{2880, 200, ENEMY_PURPLE_PLANE | SHOOT_ORAN_WIDE | MOVE_SWING, 20},

	{3120,  40, ENEMY_ALIEN_SHIP | SHOOT_BLUE_WIDE | MOVE_SWING, 0},
	{3120, 200, ENEMY_ALIEN_SHIP | SHOOT_BLUE_WIDE | MOVE_SWING, 0},
	{3140,  80, ENEMY_ALIEN_SHIP | SHOOT_BLUE_WIDE | MOVE_SWING, 10},
	{3140, 160, ENEMY_ALIEN_SHIP | SHOOT_BLUE_WIDE | MOVE_SWING, 10},
	{3160, 120, ENEMY_ALIEN_SHIP | SHOOT_BLUE_WIDE | MOVE_SWING, 20},
	{3220,  40, ENEMY_ALIEN_SHIP | SHOOT_BLUE_WIDE | MOVE_SWING, 0},
	{3220, 200, ENEMY_ALIEN_SHIP | SHOOT_BLUE_WIDE | MOVE_SWING, 0},
	{3240,  80, ENEMY_ALIEN_SHIP | SHOOT_BLUE_WIDE | MOVE_SWING, 10},
	{3240, 160, ENEMY_ALIEN_SHIP | SHOOT_BLUE_WIDE | MOVE_SWING, 10},
	{3260, 120, ENEMY_ALIEN_SHIP | SHOOT_BLUE_WIDE | MOVE_SWING, 20},

	{3420, 120, ENEMY_ALIEN_SHIP | SHOOT_BLUE_WIDE | MOVE_SWING, 20},
	{3440,  80, ENEMY_ALIEN_SHIP | SHOOT_BLUE_WIDE | MOVE_SWING, 10},
	{3440, 160, ENEMY_ALIEN_SHIP | SHOOT_BLUE_WIDE | MOVE_SWING, 10},
	{3460,  40, ENEMY_ALIEN_SHIP | SHOOT_BLUE_WIDE | MOVE_SWING, 0},
	{3460, 200, ENEMY_ALIEN_SHIP | SHOOT_BLUE_WIDE | MOVE_SWING, 0},
	{3520, 120, ENEMY_ALIEN_SHIP | SHOOT_BLUE_WIDE | MOVE_SWING, 20},
	{3540,  80, ENEMY_ALIEN_SHIP | SHOOT_BLUE_WIDE | MOVE_SWING, 10},
	{3540, 160, ENEMY_ALIEN_SHIP | SHOOT_BLUE_WIDE | MOVE_SWING, 10},
	{3560,  40, ENEMY_ALIEN_SHIP | SHOOT_BLUE_WIDE | MOVE_SWING, 0},
	{3560, 200, ENEMY_ALIEN_SHIP | SHOOT_BLUE_WIDE | MOVE_SWING, 0},

	{3700,  80, ENEMY_ORANGE_PLANE | SHOOT_ORAN_WIDE | MOVE_SWING, 0},
	{3700, 120, ENEMY_PURPLE_PLANE | SHOOT_ORAN_WIDE | MOVE_SWING, 0},
	{3740,  80, ENEMY_PURPLE_PLANE | SHOOT_ORAN_WIDE | MOVE_SWING, 0},
	{3740, 120, ENEMY_ORANGE_PLANE | SHOOT_ORAN_WIDE | MOVE_SWING, 0},

	{3800,  80, ENEMY_ALIEN_SHIP | SHOOT_ORAN_WIDE | MOVE_SWING, 10},
	{3800, 120, ENEMY_ALIEN_SHIP | SHOOT_ORAN_WIDE | MOVE_SWING, 10},
	{3860,  40, ENEMY_ALIEN_SHIP | SHOOT_BLUE_WIDE | MOVE_SWING, 30},
	{3860,  80, ENEMY_ALIEN_SHIP | SHOOT_BLUE_WIDE | MOVE_SWING, 30},
	{3860, 120, ENEMY_ALIEN_SHIP | SHOOT_BLUE_WIDE | MOVE_SWING, 30},
	{3860, 160, ENEMY_ALIEN_SHIP | SHOOT_BLUE_WIDE | MOVE_SWING, 30},

	{3960,  70, ENEMY_BIG_JT | SHOOT_ORAN_NARR | MOVE_SWING, 0},
	{3960, 130, ENEMY_BIG_JT | SHOOT_ORAN_NARR | MOVE_SWING, 0},

	{4100,  40, ENEMY_ORANGE_PLANE | SHOOT_ORAN_WIDE | MOVE_SWING,  0},
	{4130,  60, ENEMY_PURPLE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 10},
	{4160,  80, ENEMY_ORANGE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 30},
	{4190, 100, ENEMY_PURPLE_PLANE | SHOOT_ORAN_WIDE | MOVE_SWING, 40},
	{4220, 120, ENEMY_ORANGE_PLANE | SHOOT_ORAN_WIDE | MOVE_SWING, 60},
	{4250, 140, ENEMY_PURPLE_PLANE | SHOOT_ORAN_WIDE | MOVE_SWING, 40},
	{4280, 160, ENEMY_ORANGE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 30},
	{4310, 180, ENEMY_PURPLE_PLANE | SHOOT_BLUE_WIDE | MOVE_SWING, 10},
	{4340, 200, ENEMY_ORANGE_PLANE | SHOOT_ORAN_WIDE | MOVE_SWING,  0},

	{4500,  80, ENEMY_ALIEN_SHIP | SHOOT_ORAN_WIDE | MOVE_SWING, 10},
	{4500, 160, ENEMY_ALIEN_SHIP | SHOOT_ORAN_WIDE | MOVE_SWING, 20},
	{4530, 100, ENEMY_ALIEN_SHIP | SHOOT_ORAN_WIDE | MOVE_SWING, 30},
	{4530, 140, ENEMY_ALIEN_SHIP | SHOOT_ORAN_WIDE | MOVE_SWING, 10},
	{4570,  90, ENEMY_ALIEN_SHIP | SHOOT_ORAN_WIDE | MOVE_SWING, 30},
	{4570, 150, ENEMY_ALIEN_SHIP | SHOOT_ORAN_WIDE | MOVE_SWING, 10},
	{4600,  60, ENEMY_ALIEN_SHIP | SHOOT_ORAN_WIDE | MOVE_SWING, 30},
	{4600, 180, ENEMY_ALIEN_SHIP | SHOOT_ORAN_WIDE | MOVE_SWING, 10},

	{4800,  30, ENEMY_ALIEN_SHIP | SHOOT_ORAN_WIDE | MOVE_SWING, 10},
	{4800, 110, ENEMY_ALIEN_SHIP | SHOOT_ORAN_WIDE | MOVE_SWING, 20},
	{4830,  50, ENEMY_ALIEN_SHIP | SHOOT_ORAN_WIDE | MOVE_SWING, 30},
	{4830,  90, ENEMY_ALIEN_SHIP | SHOOT_ORAN_WIDE | MOVE_SWING, 10},
	{4870,  40, ENEMY_ALIEN_SHIP | SHOOT_ORAN_WIDE | MOVE_SWING, 30},
	{4870, 100, ENEMY_ALIEN_SHIP | SHOOT_ORAN_WIDE | MOVE_SWING, 10},
	{4900,  10, ENEMY_ALIEN_SHIP | SHOOT_ORAN_WIDE | MOVE_SWING, 30},
	{4900, 130, ENEMY_ALIEN_SHIP | SHOOT_ORAN_WIDE | MOVE_SWING, 10},

	{5100, 130, ENEMY_ALIEN_SHIP | SHOOT_ORAN_WIDE | MOVE_SWING, 10},
	{5100, 210, ENEMY_ALIEN_SHIP | SHOOT_ORAN_WIDE | MOVE_SWING, 20},
	{5130, 150, ENEMY_ALIEN_SHIP | SHOOT_ORAN_WIDE | MOVE_SWING, 30},
	{5130, 190, ENEMY_ALIEN_SHIP | SHOOT_ORAN_WIDE | MOVE_SWING, 10},
	{5170, 140, ENEMY_ALIEN_SHIP | SHOOT_ORAN_WIDE | MOVE_SWING, 30},
	{5170, 200, ENEMY_ALIEN_SHIP | SHOOT_ORAN_WIDE | MOVE_SWING, 10},
	{5200, 110, ENEMY_ALIEN_SHIP | SHOOT_ORAN_WIDE | MOVE_SWING, 30},
	{5200, 230, ENEMY_ALIEN_SHIP | SHOOT_ORAN_WIDE | MOVE_SWING, 10},
	{0, 0, 0, 0},
};
