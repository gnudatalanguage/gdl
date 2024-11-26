// buffer (defined by subbuffer??) contains coordinates describing the
// Hershey glyphs; findex (defined by subindex??) indexes those glyphs
// by Hershey index position; and hersh (defined by subhersh?)
// indexes the Hershey index by the plpoint "ascii" index that in the
// range from 0 to 31 points to Hershey indices that point to useful
// plotting symbols and from 32 to 126 indexes Hershey indices that
// point to ascii printable characters.

//      short int findex[3000]
//      short int buffer[27000]
//      short int hersh[0:175,4]

extern short int
    subindex00[], subindex01[], subindex02[], subindex03[], subindex04[],
    subindex05[], subindex06[], subindex07[], subindex08[], subindex09[],
    subindex10[], subindex11[], subindex12[], subindex13[], subindex14[],
    subindex15[], subindex16[], subindex17[], subindex18[], subindex19[],
    subindex20[], subindex21[], subindex22[], subindex23[], subindex24[],
    subindex25[], subindex26[], subindex27[], subindex28[], subindex29[];

short int *findex[30] = {
    subindex00, subindex01, subindex02, subindex03, subindex04,
    subindex05, subindex06, subindex07, subindex08, subindex09,
    subindex10, subindex11, subindex12, subindex13, subindex14,
    subindex15, subindex16, subindex17, subindex18, subindex19,
    subindex20, subindex21, subindex22, subindex23, subindex24,
    subindex25, subindex26, subindex27, subindex28, subindex29
};

extern short int
    subbuffer000[], subbuffer001[], subbuffer002[], subbuffer003[],
    subbuffer004[], subbuffer005[], subbuffer006[], subbuffer007[],
    subbuffer008[], subbuffer009[], subbuffer010[], subbuffer011[],
    subbuffer012[], subbuffer013[], subbuffer014[], subbuffer015[],
    subbuffer016[], subbuffer017[], subbuffer018[], subbuffer019[],
    subbuffer020[], subbuffer021[], subbuffer022[], subbuffer023[],
    subbuffer024[], subbuffer025[], subbuffer026[], subbuffer027[],
    subbuffer028[], subbuffer029[], subbuffer030[], subbuffer031[],
    subbuffer032[], subbuffer033[], subbuffer034[], subbuffer035[],
    subbuffer036[], subbuffer037[], subbuffer038[], subbuffer039[],
    subbuffer040[], subbuffer041[], subbuffer042[], subbuffer043[],
    subbuffer044[], subbuffer045[], subbuffer046[], subbuffer047[],
    subbuffer048[], subbuffer049[], subbuffer050[], subbuffer051[],
    subbuffer052[], subbuffer053[], subbuffer054[], subbuffer055[],
    subbuffer056[], subbuffer057[], subbuffer058[], subbuffer059[],
    subbuffer060[], subbuffer061[], subbuffer062[], subbuffer063[],
    subbuffer064[], subbuffer065[], subbuffer066[], subbuffer067[],
    subbuffer068[], subbuffer069[], subbuffer070[], subbuffer071[],
    subbuffer072[], subbuffer073[], subbuffer074[], subbuffer075[],
    subbuffer076[], subbuffer077[], subbuffer078[], subbuffer079[],
    subbuffer080[], subbuffer081[], subbuffer082[], subbuffer083[],
    subbuffer084[], subbuffer085[], subbuffer086[], subbuffer087[],
    subbuffer088[], subbuffer089[], subbuffer090[], subbuffer091[],
    subbuffer092[], subbuffer093[], subbuffer094[], subbuffer095[],
    subbuffer096[], subbuffer097[], subbuffer098[], subbuffer099[],
    subbuffer100[], subbuffer101[], subbuffer102[], subbuffer103[],
    subbuffer104[], subbuffer105[], subbuffer106[], subbuffer107[],
    subbuffer108[], subbuffer109[], subbuffer110[], subbuffer111[],
    subbuffer112[], subbuffer113[], subbuffer114[], subbuffer115[],
    subbuffer116[], subbuffer117[], subbuffer118[], subbuffer119[],
    subbuffer120[], subbuffer121[], subbuffer122[], subbuffer123[],
    subbuffer124[], subbuffer125[], subbuffer126[], subbuffer127[],
    subbuffer128[], subbuffer129[], subbuffer130[], subbuffer131[],
    subbuffer132[], subbuffer133[], subbuffer134[], subbuffer135[],
    subbuffer136[], subbuffer137[], subbuffer138[], subbuffer139[],
    subbuffer140[], subbuffer141[], subbuffer142[], subbuffer143[],
    subbuffer144[], subbuffer145[], subbuffer146[], subbuffer147[],
    subbuffer148[], subbuffer149[], subbuffer150[], subbuffer151[],
    subbuffer152[], subbuffer153[], subbuffer154[], subbuffer155[],
    subbuffer156[], subbuffer157[], subbuffer158[], subbuffer159[],
    subbuffer160[], subbuffer161[], subbuffer162[], subbuffer163[],
    subbuffer164[], subbuffer165[], subbuffer166[], subbuffer167[],
    subbuffer168[], subbuffer169[], subbuffer170[], subbuffer171[],
    subbuffer172[], subbuffer173[], subbuffer174[], subbuffer175[],
    subbuffer176[], subbuffer177[], subbuffer178[], subbuffer179[],
    subbuffer180[], subbuffer181[], subbuffer182[], subbuffer183[],
    subbuffer184[], subbuffer185[], subbuffer186[], subbuffer187[],
    subbuffer188[], subbuffer189[], subbuffer190[], subbuffer191[],
    subbuffer192[], subbuffer193[], subbuffer194[], subbuffer195[],
    subbuffer196[], subbuffer197[], subbuffer198[], subbuffer199[],
    subbuffer200[], subbuffer201[], subbuffer202[], subbuffer203[],
    subbuffer204[], subbuffer205[], subbuffer206[], subbuffer207[],
    subbuffer208[], subbuffer209[], subbuffer210[], subbuffer211[],
    subbuffer212[], subbuffer213[], subbuffer214[], subbuffer215[],
    subbuffer216[], subbuffer217[], subbuffer218[], subbuffer219[],
    subbuffer220[], subbuffer221[], subbuffer222[], subbuffer223[],
    subbuffer224[], subbuffer225[], subbuffer226[], subbuffer227[],
    subbuffer228[], subbuffer229[], subbuffer230[], subbuffer231[],
    subbuffer232[], subbuffer233[], subbuffer234[], subbuffer235[],
    subbuffer236[], subbuffer237[], subbuffer238[], subbuffer239[],
    subbuffer240[], subbuffer241[], subbuffer242[], subbuffer243[],
    subbuffer244[], subbuffer245[], subbuffer246[], subbuffer247[],
    subbuffer248[], subbuffer249[], subbuffer250[], subbuffer251[],
    subbuffer252[], subbuffer253[], subbuffer254[], subbuffer255[],
    subbuffer256[], subbuffer257[], subbuffer258[], subbuffer259[],
    subbuffer260[], subbuffer261[], subbuffer262[], subbuffer263[],
    subbuffer264[], subbuffer265[], subbuffer266[], subbuffer267[],
    subbuffer268[], subbuffer269[];

short int *buffer[270] = {
    subbuffer000, subbuffer001, subbuffer002, subbuffer003, subbuffer004,
    subbuffer005, subbuffer006, subbuffer007, subbuffer008, subbuffer009,
    subbuffer010, subbuffer011, subbuffer012, subbuffer013, subbuffer014,
    subbuffer015, subbuffer016, subbuffer017, subbuffer018, subbuffer019,
    subbuffer020, subbuffer021, subbuffer022, subbuffer023, subbuffer024,
    subbuffer025, subbuffer026, subbuffer027, subbuffer028, subbuffer029,
    subbuffer030, subbuffer031, subbuffer032, subbuffer033, subbuffer034,
    subbuffer035, subbuffer036, subbuffer037, subbuffer038, subbuffer039,
    subbuffer040, subbuffer041, subbuffer042, subbuffer043, subbuffer044,
    subbuffer045, subbuffer046, subbuffer047, subbuffer048, subbuffer049,
    subbuffer050, subbuffer051, subbuffer052, subbuffer053, subbuffer054,
    subbuffer055, subbuffer056, subbuffer057, subbuffer058, subbuffer059,
    subbuffer060, subbuffer061, subbuffer062, subbuffer063, subbuffer064,
    subbuffer065, subbuffer066, subbuffer067, subbuffer068, subbuffer069,
    subbuffer070, subbuffer071, subbuffer072, subbuffer073, subbuffer074,
    subbuffer075, subbuffer076, subbuffer077, subbuffer078, subbuffer079,
    subbuffer080, subbuffer081, subbuffer082, subbuffer083, subbuffer084,
    subbuffer085, subbuffer086, subbuffer087, subbuffer088, subbuffer089,
    subbuffer090, subbuffer091, subbuffer092, subbuffer093, subbuffer094,
    subbuffer095, subbuffer096, subbuffer097, subbuffer098, subbuffer099,
    subbuffer100, subbuffer101, subbuffer102, subbuffer103, subbuffer104,
    subbuffer105, subbuffer106, subbuffer107, subbuffer108, subbuffer109,
    subbuffer110, subbuffer111, subbuffer112, subbuffer113, subbuffer114,
    subbuffer115, subbuffer116, subbuffer117, subbuffer118, subbuffer119,
    subbuffer120, subbuffer121, subbuffer122, subbuffer123, subbuffer124,
    subbuffer125, subbuffer126, subbuffer127, subbuffer128, subbuffer129,
    subbuffer130, subbuffer131, subbuffer132, subbuffer133, subbuffer134,
    subbuffer135, subbuffer136, subbuffer137, subbuffer138, subbuffer139,
    subbuffer140, subbuffer141, subbuffer142, subbuffer143, subbuffer144,
    subbuffer145, subbuffer146, subbuffer147, subbuffer148, subbuffer149,
    subbuffer150, subbuffer151, subbuffer152, subbuffer153, subbuffer154,
    subbuffer155, subbuffer156, subbuffer157, subbuffer158, subbuffer159,
    subbuffer160, subbuffer161, subbuffer162, subbuffer163, subbuffer164,
    subbuffer165, subbuffer166, subbuffer167, subbuffer168, subbuffer169,
    subbuffer170, subbuffer171, subbuffer172, subbuffer173, subbuffer174,
    subbuffer175, subbuffer176, subbuffer177, subbuffer178, subbuffer179,
    subbuffer180, subbuffer181, subbuffer182, subbuffer183, subbuffer184,
    subbuffer185, subbuffer186, subbuffer187, subbuffer188, subbuffer189,
    subbuffer190, subbuffer191, subbuffer192, subbuffer193, subbuffer194,
    subbuffer195, subbuffer196, subbuffer197, subbuffer198, subbuffer199,
    subbuffer200, subbuffer201, subbuffer202, subbuffer203, subbuffer204,
    subbuffer205, subbuffer206, subbuffer207, subbuffer208, subbuffer209,
    subbuffer210, subbuffer211, subbuffer212, subbuffer213, subbuffer214,
    subbuffer215, subbuffer216, subbuffer217, subbuffer218, subbuffer219,
    subbuffer220, subbuffer221, subbuffer222, subbuffer223, subbuffer224,
    subbuffer225, subbuffer226, subbuffer227, subbuffer228, subbuffer229,
    subbuffer230, subbuffer231, subbuffer232, subbuffer233, subbuffer234,
    subbuffer235, subbuffer236, subbuffer237, subbuffer238, subbuffer239,
    subbuffer240, subbuffer241, subbuffer242, subbuffer243, subbuffer244,
    subbuffer245, subbuffer246, subbuffer247, subbuffer248, subbuffer249,
    subbuffer250, subbuffer251, subbuffer252, subbuffer253, subbuffer254,
    subbuffer255, subbuffer256, subbuffer257, subbuffer258, subbuffer259,
    subbuffer260, subbuffer261, subbuffer262, subbuffer263, subbuffer264,
    subbuffer265, subbuffer266, subbuffer267, subbuffer268, subbuffer269
};

extern short int
          subhersh0[], subhersh1[], subhersh2[], subhersh3[];

short int *hersh[4] = {
    subhersh0, subhersh1, subhersh2, subhersh3
};

short int subindex00[100] = {
    1,    13,  32,  46,  61,  76,  88, 106, 118, 124,
    134, 146, 155, 170, 182, 199, 212, 232, 248, 264,
    273, 285, 294, 309, 318, 328, 340, 352, 371, 380,
    392, 407, 419, 431, 454, 460, 472, 481, 496, 508,
    523, 540, 552, 565, 578, 587, 605, 621, 630, 645,
    0,     0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,     0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,     0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,     0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,     0,   0,   0,   0,   0,   0,   0,   0, 0
};
short int subindex01[100] = {
    0, 0, 0, 0, 0, 0,   0,   0,   0, 0,
    0, 0, 0, 0, 0, 0,   0,   0,   0, 0,
    0, 0, 0, 0, 0, 0,   0,   0,   0, 0,
    0, 0, 0, 0, 0, 0,   0,   0,   0, 0,
    0, 0, 0, 0, 0, 0,   0,   0,   0, 0,
    0, 0, 0, 0, 0, 0,   0,   0,   0, 0,
    0, 0, 0, 0, 0, 0,   0,   0,   0, 0,
    0, 0, 0, 0, 0, 0,   0,   0,   0, 0,
    0, 0, 0, 0, 0, 0,   0,   0,   0, 0,
    0, 0, 0, 0, 0, 0, 661, 666, 671, 676
};
short int subindex02[100] = {
    691,   698,  710,  728,  738,  755,  772,  781,  803,  820,
    829,   839,  854,  870,  885,  905,  911,  920,  933,  952,
    958,   968,  978,  984,  990,  999, 1008, 1017, 1029, 1038,
    1048, 1058, 1068, 1083, 1107, 1130,    0, 1154, 1169, 1204,
    0,    1210,    0,    0,    0,    0,    0, 1219,    0, 1266,
    0,    1297,    0, 1308,    0, 1317,    0, 1349, 1355,    0,
    1364, 1415, 1470, 1522, 1589, 1669, 1765, 1855, 1934, 2009,
    2038, 2096, 2138, 2200, 2249, 2264,    0, 2285, 2311, 2337,
    2356, 2375,    0, 2394,    0,    0,    0,    0,    0,    0,
    0,       0,    0,    0,    0,    0,    0,    0,    0, 0
};
short int subindex03[100] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
short int subindex04[100] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
short int subindex05[100] = {
    2406, 2418, 2445, 2467, 2486, 2501, 2513, 2539, 2551, 2557,
    2571, 2583, 2592, 2607, 2619, 2644, 2661, 2689, 2709, 2733,
    2742, 2756, 2765, 2780, 2789, 2799, 2811, 2823, 2850, 2859,
    2871, 2886, 2898, 2910, 2938, 2944, 2956, 2965, 2980, 2992,
    3004, 3029, 3041, 3058, 3071, 3080, 3102, 3126, 3135, 3156,
    3176, 3199, 3243, 3270, 3308, 3339, 3370, 3402, 3443, 3471,
    3499, 3541, 3573, 3621, 3656, 3688, 3722, 3757, 3798, 3829,
    3857, 3893, 3928, 3956, 3995, 4036,    0,    0,    0,    0,
    0,       0, 4079,    0,    0,    0,    0,    0,    0,    0,
    0,       0,    0,    0,    0,    0,    0,    0,    0, 0
};
short int subindex06[100] = {
    4091, 4112, 4133, 4151, 4172, 4193, 4205, 4231, 4245, 4257,
    4272, 4284, 4290, 4312, 4326, 4347, 4368, 4389, 4401, 4422,
    4434, 4448, 4457, 4472, 4481, 4494, 4506, 4533, 4567, 4587,
    4614, 4636, 4662, 4684, 4714, 4726, 4748, 4760, 4784, 4801,
    4833, 4854, 4870, 4892, 4913, 4924, 4943, 4967, 4984, 5007,
    5033, 5058, 5084, 5101, 5128, 5148, 5175, 5206, 5238, 5257,
    5280, 5316, 5337, 5373, 5399, 5425, 5452, 5482, 5500, 5519,
    5538, 5560, 5580, 5608, 5631, 5657, 5683,    0,    0,    0,
    0,       0, 5708, 5737, 5756, 5782, 5806,    0,    0,    0,
    0,       0,    0,    0,    0,    0, 5825, 5830, 5835, 5840
};
short int subindex07[100] = {
    5861, 5869, 5887, 5906,  5916, 5937, 5964, 5973, 6006, 6033,
    6042, 6054, 6069, 6087,  6099, 6123, 6129, 6138, 6155, 6185,
    6191, 6205, 6219, 6225,  6231, 6240, 6249, 6258, 6270, 6279,
    6290, 6301, 6313, 6328,  6366,    0, 6397, 6406, 6415, 6422,
    6443, 6479, 6508, 6531,  6582, 6640,    0,    0,    0, 6698,
    6719, 6738, 6750, 6781,  6810, 6827, 6840, 6854, 6870, 6884,
    6894, 6903, 6913, 6923,  6947, 6971, 7005, 7030,    0,    0,
    0,       0,    0,    0,     0,    0,    0,    0,    0,    0,
    0,       0,    0,    0,     0,    0,    0,    0,    0,    0,
    0,       0,    0,    0, 26692, 7063, 7069, 7075, 7081, 7087
};
short int subindex08[100] = {
    7093, 7099, 7105, 7111, 7117, 7123, 7129, 7135, 7141, 7147,
    7158, 7169, 7180, 7191, 7203, 7215, 7227, 7239, 7247, 7255,
    7263, 7271, 7297, 7323, 7349, 7375, 7399, 7421, 7450, 7464,
    7476, 7484, 7493, 7505,    0,    0,    0,    0,    0, 7517,
    7538, 7547, 7555, 7564, 7579, 7588, 7597,    0,    0, 7609,
    7647, 7677, 7697, 7717, 7737, 7757, 7782,    0,    0, 7796,
    7812, 7826, 7843, 7864, 7888, 7902, 7919, 7954, 7967, 8004,
    8071, 8168, 8211, 8246,    0,    0,    0,    0,    0,    0,
    0,       0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,       0,    0,    0,    0,    0,    0,    0, 8264, 8273
};
short int subindex09[100] = {
    8286, 8303, 8324, 8345, 8370, 8407, 8444, 8497, 8534, 8570,
    0,       0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,       0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,       0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,       0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,       0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,       0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,       0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,       0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,       0,    0,    0,    0,    0,    0,    0,    0, 0
};
short int subindex10[100] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
short int subindex11[100] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
short int subindex12[100] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
short int subindex13[100] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
short int subindex14[100] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
short int subindex15[100] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
short int subindex16[100] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
short int subindex17[100] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
short int subindex18[100] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
short int subindex19[100] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
short int subindex20[100] = {
    8604,   8625,  8673,  8708,  8741,  8766,  8789,  8832,  8862,  8877,
    8900,   8930,  8947,  8980,  9004,  9051,  9083,  9150,  9198,  9235,
    9254,   9280,  9298,  9325,  9349,  9372,  9391,  9412,  9460,  9477,
    9495,   9520,  9539,  9569,  9628,  9643,  9673,  9691,  9724,  9748,
    9787,   9834,  9858,  9890,  9913,  9932,  9968, 10019, 10043, 10087,
    10133, 10154, 10198, 10235, 10268, 10293, 10316, 10361, 10391, 10406,
    10430, 10460, 10477, 10510, 10534, 10579, 10609, 10673, 10718, 10756,
    10775, 10803, 10821, 10848, 10872, 10895, 10914,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0, 0
};
short int subindex21[100] = {
    10963, 11005, 11041, 11072, 11111, 11145, 11170, 11233, 11264, 11285,
    11313, 11343, 11358, 11405, 11436, 11475, 11514, 11550, 11576, 11611,
    11630, 11661, 11679, 11706, 11730, 11755, 11774, 11817, 11877, 11908,
    11955, 11990, 12024, 12059, 12106, 12124, 12156, 12182, 12213, 12240,
    12288, 12323, 12348, 12382, 12420, 12439, 12473, 12513, 12539, 12576,
    12623, 12665, 12704, 12732, 12777, 12806, 12844, 12890, 12924, 12953,
    12988, 13025, 13046, 13101, 13141, 13176, 13221, 13257, 13286, 13317,
    13338, 13378, 13407, 13451, 13496, 13540, 13574, 13620, 13656, 13694,
    13751, 13810,     0, 13825, 13853, 13899, 13944,     0,     0, 13974,
    14022, 14094, 14149, 14206, 14295, 14386, 14409, 14414, 14419, 14424
};
short int subindex22[100] = {
    14467, 14481, 14529, 14579, 14595, 14637, 14688, 14722, 14788, 14839,
    14848, 14859, 14874, 14891, 14909, 14944, 14953, 14968, 14985, 14997,
    15003, 15026, 15049, 15064, 15079, 15108, 15137, 15144, 15151, 15157,
    15166, 15172, 15181, 15193, 15205, 15214, 15223, 15241, 15250, 15262,
    15274, 15281, 15288, 15301, 15314, 15338, 15365, 15376, 15386, 15396,
    15417, 15428, 15439, 15450, 15461, 15474, 15490, 15506, 15522, 15538,
    15557, 15571, 15585, 15599, 15613, 15660, 15678, 15691, 15726, 15779,
    15808, 15843, 15895, 15954, 15999, 16014, 16063, 16096, 16155,     0,
    16170, 16211, 16259, 16289, 16320, 16354, 16385, 16418, 16465, 16505,
    16539, 16566, 16596, 16611, 16662, 16713, 16729, 16745, 16761, 0
};
short int subindex23[100] = {
    16777, 16820, 16874, 16907, 16958, 17009, 17060, 17104, 17137, 17173,
    17209, 17248,     0,     0,     0,     0, 17299, 17314, 17335, 17356,
    17395, 17434, 17471, 17492, 17513, 17541, 17562, 17583, 17635, 17664,
    17753, 17859,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0, 17892, 17907, 17928, 17949,
    17984, 18023, 18060, 18081, 18102, 18130, 18151, 18172, 18211, 18240,
    18329, 18421,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0, 0
};
short int subindex24[100] = {
    18509, 18533, 18559, 18590, 18621, 18638, 18655, 18696, 18737, 18772,
    18807, 18824,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0, 0
};
short int subindex25[100] = {
    18884, 18907, 18954, 18995, 19030, 19060, 19084, 19131, 19156, 19167,
    19190, 19215, 19232, 19261, 19284, 19327, 19357, 19408, 19445, 19491,
    19509, 19536, 19553, 19582, 19601, 19621,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    19644, 19685, 19758, 19802, 19848, 19900, 19949, 20005, 20063, 20099,
    20136, 20198, 20241, 20293, 20334, 20371, 20427, 20473, 20541, 20580,
    20621, 20663, 20702, 20746, 20801, 20846,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0, 0
};
short int subindex26[100] = {
    20893, 20932, 20971, 21006, 21045, 21084, 21111, 21162, 21190, 21217,
    21244, 21269, 21280, 21325, 21353, 21392, 21431, 21470, 21494, 21547,
    21566, 21594, 21611, 21640, 21659, 21679,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    21702, 21738, 21764, 21789, 21825, 21851, 21881, 21922, 21955, 21977,
    22004, 22038, 22058, 22106, 22141, 22173, 22206, 22244, 22271, 22296,
    22319, 22349, 22375, 22414, 22459, 22494,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0, 22535, 22540, 22545, 22550
};
short int subindex27[100] = {
    22595, 22610, 22647, 22698, 22719, 22775, 22840, 22855, 22926, 22991,
    23010, 23037, 23072, 23115, 23142, 23203, 23230, 23257, 23331, 23385,
    23396, 23423, 23450, 23492, 23503, 23522, 23541, 23555, 23580,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0, 23613, 23618, 23623, 23628,
    23673, 23691, 23736, 23789, 23802, 23844, 23893, 23926, 23992, 24041,
    24050, 24061, 24075, 24092, 24112, 24149, 24160, 24171, 24229, 24273,
    24279, 24302, 24325, 24337, 24343, 24352, 24361, 24370, 24385,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0, 0
};
short int subindex28[100] = {
    24402, 24423, 24457, 24505, 24522, 24556, 24581, 24655, 24703, 24733,
    24773, 24819, 24847, 24880, 24910, 24957, 24981, 25013, 25048, 25067,
    25094, 25145, 25169, 25199, 25230, 25263, 25302, 25336, 25380, 25412,
    25454, 25516,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0, 0
};
short int subindex29[100] = {
    25564, 25606, 25657, 25697, 25714, 25739, 25773, 25835, 25880, 25910,
    25950, 25991, 26016, 26049, 26079, 26118, 26142, 26181, 26212, 26231,
    26256, 26311, 26335, 26361, 26392, 26425, 26460, 26490, 26530, 26558,
    26595, 26649,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,         0,     0,     0,     0,     0,     0,     0,     0, 0
};

short int subbuffer000[100] = {
    -4,   7749, 7621, 8261, 7740,   64, 8261, 8764,   64, 7999,
    8511,    0,   -4, 7749, 7621, 7877, 7868,   64, 7877, 8389,
    8644, 8642, 8385,   64, 7873, 8385, 8640, 8637, 8380, 7868,
    0,      -4, 7749, 7622, 8772, 8517, 8261, 8004, 7874, 7871,
    7997, 8252, 8508, 8765,    0,   -4, 7749, 7621, 7877, 7868,
    64,   7877, 8261, 8516, 8642, 8639, 8509, 8252, 7868,    0,
    -4,   7749, 7621, 7877, 7868,   64, 7877, 8645,   64, 7873,
    8385,   64, 7868, 8636,    0,   -4, 7749, 7620, 7877, 7868,
    64,   7877, 8645,   64, 7873, 8385,    0,   -4, 7749, 7622,
    8772, 8517, 8261, 8004, 7874, 7871, 7997, 8252, 8508, 8765
};
short int subbuffer001[100] = {
    8768,   64, 8384, 8768,    0,   -4, 7749, 7621, 7877, 7868,
    64,   8645, 8636,   64, 7873, 8641,    0,   -4, 7749, 8002,
    8261, 8252,    0,   -4, 7749, 7747, 8389, 8382, 8252, 7996,
    7870, 7871,    0,   -4, 7749, 7621, 7877, 7868,   64, 8645,
    7871,   64, 8129, 8636,    0,   -4, 7749, 7620, 7877, 7868,
    64,   7868, 8636,    0,   -4, 7749, 7494, 7749, 7740,   64,
    7749, 8252,   64, 8773, 8252,   64, 8773, 8764,    0,   -4,
    7749, 7621, 7877, 7868,   64, 7877, 8636,   64, 8645, 8636,
    0,      -4, 7749, 7622, 8261, 8004, 7874, 7871, 7997, 8252,
    8380, 8637, 8767, 8770, 8644, 8389, 8261,    0,   -4, 7749
};
