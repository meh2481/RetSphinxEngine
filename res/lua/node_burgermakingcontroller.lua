local node_burgermakingcontroller = {}
node_burgermakingcontroller.__index = node_burgermakingcontroller

--Notes that are enough to experiment with
local notes = {
	{dir="right", pos=0.938},
	{dir="right", pos=1.237},
	{dir="up", pos=1.557},
	{dir="left", pos=1.855},
	{dir="right", pos=4.373},
	{dir="right", pos=4.714},
	{dir="up", pos=5.098},
	{dir="up", pos=5.354},
	{dir="down", pos=5.823},
	{dir="up", pos=7.146},
	{dir="up", pos=7.487},
	{dir="up", pos=7.807},
	{dir="down", pos=7.999},
	{dir="down", pos=8.341},
	{dir="down", pos=8.639},
	{dir="right", pos=10.581},
	{dir="right", pos=10.943},
	{dir="right", pos=11.221},
	{dir="left", pos=11.498},
	{dir="left", pos=11.797},
	{dir="left", pos=12.053},
	{dir="up", pos=14.058},
	{dir="right", pos=14.335},
	{dir="down", pos=14.613},
	{dir="left", pos=14.869},
	{dir="up", pos=15.189},
	{dir="right", pos=15.423},
	{dir="right", pos=17.386},
	{dir="up", pos=17.706},
	{dir="left", pos=18.047},
	{dir="down", pos=18.346},
	{dir="right", pos=18.602},
	{dir="up", pos=18.901},
	{dir="up", pos=20.906},
	{dir="down", pos=21.29},
	{dir="up", pos=21.717},
	{dir="down", pos=22.143},
	{dir="up", pos=22.57},
	{dir="down", pos=23.039},
	{dir="up", pos=23.423},
	{dir="down", pos=23.85},
	{dir="up", pos=24.277},
	{dir="down", pos=24.725},
	{dir="up", pos=25.151},
	{dir="down", pos=25.599},
	{dir="left", pos=26.047},
	{dir="right", pos=26.175},
	{dir="left", pos=26.325},
	{dir="right", pos=26.431},
	{dir="left", pos=26.559},
	{dir="right", pos=26.666},
	{dir="left", pos=26.837},
	{dir="up", pos=27.69},
	{dir="right", pos=28.138},
	{dir="down", pos=28.565},
	{dir="left", pos=28.991},
	{dir="down", pos=29.439},
	{dir="right", pos=29.845},
	{dir="up", pos=30.271},
	{dir="left", pos=30.677},
	{dir="down", pos=31.189},
	{dir="right", pos=31.573},
	{dir="up", pos=31.999},
	{dir="right", pos=32.426},
	{dir="down", pos=32.831},
	{dir="left", pos=33.279},
	{dir="up", pos=33.706},
	{dir="right", pos=33.983},
	{dir="down", pos=34.175},
	{dir="left", pos=34.367},
	{dir="up", pos=34.581},
	{dir="down", pos=35.029},
	{dir="up", pos=35.434},
	{dir="down", pos=35.839},
	{dir="up", pos=36.309},
	{dir="down", pos=36.714},
	{dir="up", pos=37.141},
	{dir="down", pos=37.525},
	{dir="left", pos=37.951},
	{dir="right", pos=38.421},
	{dir="left", pos=38.847},
	{dir="right", pos=39.295},
	{dir="up", pos=39.722},
	{dir="down", pos=40.149},
	{dir="left", pos=40.575},
	{dir="up", pos=40.81},
	{dir="right", pos=41.045},
	{dir="down", pos=41.237},
	{dir="left", pos=41.471},
	{dir="right", pos=41.898},
	{dir="left", pos=42.303},
	{dir="right", pos=42.709},
	{dir="left", pos=43.178},
	{dir="right", pos=43.605},
	{dir="left", pos=44.01},
	{dir="right", pos=44.437},
	{dir="left", pos=44.885},
	{dir="right", pos=45.29},
	{dir="left", pos=45.738},
	{dir="right", pos=46.122},
	{dir="left", pos=46.549},
	{dir="right", pos=46.997},
	{dir="up", pos=47.423},
	{dir="right", pos=47.679},
	{dir="down", pos=47.914},
	{dir="left", pos=48.085},
	{dir="up", pos=48.319},
	{dir="down", pos=48.703},
	{dir="up", pos=49.13},
	{dir="down", pos=49.535},
	{dir="up", pos=49.983},
	{dir="down", pos=50.431},
	{dir="up", pos=50.858},
	{dir="down", pos=51.285},
	{dir="up", pos=51.711},
	{dir="down", pos=52.117},
	{dir="up", pos=52.586},
	{dir="down", pos=53.013},
	{dir="up", pos=53.418},
	{dir="down", pos=53.866},
	{dir="left", pos=54.271},
	{dir="up", pos=54.485},
	{dir="right", pos=54.741},
	{dir="down", pos=54.933},
	{dir="left", pos=55.167},
	{dir="up", pos=58.623},
	{dir="down", pos=59.05},
	{dir="up", pos=59.455},
	{dir="down", pos=59.839},
	{dir="left", pos=60.223},
	{dir="right", pos=60.671},
	{dir="left", pos=61.119},
	{dir="right", pos=61.546},
	{dir="left", pos=65.45},
	{dir="right", pos=65.898},
	{dir="left", pos=66.303},
	{dir="right", pos=66.751},
	{dir="left", pos=67.157},
	{dir="right", pos=67.583},
	{dir="up", pos=68.01},
	{dir="up", pos=68.287},
	{dir="down", pos=68.479},
	{dir="down", pos=68.65},
	{dir="down", pos=68.821},
	{dir="down", pos=68.949},
	{dir="left", pos=69.333},
	{dir="right", pos=69.759},
	{dir="up", pos=70.186},
	{dir="down", pos=70.591},
	{dir="left", pos=70.997},
	{dir="right", pos=71.402},
	{dir="up", pos=71.893},
	{dir="down", pos=72.298},
	{dir="up", pos=72.725},
	{dir="up", pos=73.173},
	{dir="up", pos=73.621},
	{dir="down", pos=73.983},
	{dir="left", pos=74.367},
	{dir="right", pos=74.815},
	{dir="up", pos=75.242},
	{dir="down", pos=75.69},
	{dir="down", pos=76.159},
	{dir="left", pos=76.586},
	{dir="right", pos=76.97},
	{dir="up", pos=77.418},
	{dir="left", pos=77.866},
	{dir="down", pos=78.293},
	{dir="right", pos=78.741},
	{dir="up", pos=79.253},
	{dir="down", pos=79.615},
	{dir="up", pos=80.042},
	{dir="down", pos=80.469},
	{dir="up", pos=80.895},
	{dir="down", pos=81.322},
	{dir="up", pos=81.727},
	{dir="down", pos=82.111},
	{dir="up", pos=82.602},
	{dir="down", pos=84.202},
	{dir="down", pos=84.394},
	{dir="down", pos=84.693},
	{dir="down", pos=84.991},
	{dir="down", pos=85.333},
	{dir="down", pos=85.631},
	{dir="right", pos=86.186},
	{dir="left", pos=87.573},
	{dir="left", pos=87.786},
	{dir="left", pos=88.106},
	{dir="left", pos=88.405},
	{dir="left", pos=88.767},
	{dir="left", pos=89.002},
	{dir="up", pos=89.514},
	{dir="down", pos=89.898},
	{dir="up", pos=90.325},
	{dir="down", pos=90.773},
	{dir="up", pos=91.178},
	{dir="down", pos=91.541},
	{dir="up", pos=92.01},
	{dir="down", pos=92.437},
	{dir="left", pos=92.906},
	{dir="right", pos=93.333},
	{dir="left", pos=93.759},
	{dir="right", pos=94.186},
	{dir="left", pos=94.613},
	{dir="right", pos=95.018},
	{dir="up", pos=95.445},
	{dir="up", pos=95.722},
	{dir="up", pos=95.999},
	{dir="up", pos=96.127},
	{dir="up", pos=96.255},
	{dir="up", pos=96.383},
	{dir="down", pos=96.789},
	{dir="up", pos=97.151},
	{dir="down", pos=97.599},
	{dir="up", pos=98.047},
	{dir="left", pos=98.453},
	{dir="right", pos=98.922},
	{dir="left", pos=99.327},
	{dir="up", pos=99.754},
	{dir="down", pos=100.159},
	{dir="right", pos=100.565},
	{dir="up", pos=100.97},
	{dir="left", pos=101.418},
	{dir="down", pos=101.845},
	{dir="right", pos=102.314},
	{dir="left", pos=103.146},
	{dir="right", pos=103.381},
	{dir="left", pos=103.594},
	{dir="right", pos=103.829},
	{dir="left", pos=104.042},
	{dir="right", pos=104.213},
	{dir="up", pos=104.661},
	{dir="down", pos=105.066},
	{dir="up", pos=105.279},
	{dir="down", pos=105.45},
	{dir="up", pos=105.685},
	{dir="down", pos=105.855},
	{dir="down", pos=106.623},
	{dir="left", pos=107.029},
	{dir="right", pos=107.434},
	{dir="up", pos=107.839},
	{dir="down", pos=108.266},
	{dir="left", pos=108.65},
	{dir="right", pos=109.183},
	{dir="up", pos=109.61},
	{dir="down", pos=110.122},
	{dir="down", pos=110.421},
	{dir="down", pos=110.719},
	{dir="down", pos=110.933},
	{dir="down", pos=111.21},
	{dir="down", pos=111.53},
	{dir="right", pos=113.45},
	{dir="right", pos=113.791},
	{dir="right", pos=114.09},
	{dir="right", pos=114.303},
	{dir="right", pos=114.645},
	{dir="right", pos=114.965},
	{dir="left", pos=116.906},
	{dir="left", pos=117.205},
	{dir="left", pos=117.525},
	{dir="left", pos=117.759},
	{dir="left", pos=118.079},
	{dir="left", pos=118.399},
	{dir="up", pos=118.634},
	{dir="up", pos=118.954},
	{dir="up", pos=119.253},
	{dir="up", pos=119.509},
	{dir="up", pos=119.85},
	{dir="up", pos=120.149},
	{dir="down", pos=120.362},
	{dir="down", pos=120.682},
	{dir="down", pos=120.959},
	{dir="down", pos=121.194},
	{dir="down", pos=121.535},
	{dir="down", pos=121.77},
	{dir="right", pos=122.09},
	{dir="right", pos=122.367},
	{dir="right", pos=122.687},
	{dir="up", pos=122.879},
	{dir="left", pos=123.178},
	{dir="down", pos=123.349},
	{dir="right", pos=123.605},
	{dir="up", pos=123.775},
	{dir="up", pos=124.202},
	{dir="left", pos=124.586},
	{dir="right", pos=125.013},
	{dir="up", pos=125.439},
	{dir="down", pos=125.845},
	{dir="left", pos=126.271},
	{dir="right", pos=126.719},
	{dir="up", pos=127.125},
	{dir="down", pos=127.573},
	{dir="left", pos=127.999},
	{dir="right", pos=128.447},
	{dir="left", pos=128.895},
	{dir="right", pos=129.343},
	{dir="up", pos=129.685},
	{dir="down", pos=129.877},
	{dir="left", pos=130.069},
	{dir="right", pos=130.325},
	{dir="up", pos=130.538},
	{dir="down", pos=131.007},
	{dir="left", pos=131.413},
	{dir="right", pos=131.839},
	{dir="up", pos=132.287},
	{dir="down", pos=132.65},
	{dir="left", pos=133.098},
	{dir="right", pos=133.546},
	{dir="up", pos=133.973},
	{dir="down", pos=134.399},
	{dir="left", pos=134.847},
	{dir="right", pos=135.274},
	{dir="up", pos=135.722},
	{dir="down", pos=136.149},
	{dir="left", pos=136.533},
	{dir="right", pos=136.746},
	{dir="up", pos=136.917},
	{dir="down", pos=137.151},
	{dir="left", pos=137.322},
	{dir="up", pos=137.599},
	{dir="down", pos=137.962},
	{dir="up", pos=138.282},
	{dir="down", pos=138.709},
	{dir="up", pos=139.135},
	{dir="up", pos=139.605},
	{dir="down", pos=140.031},
	{dir="up", pos=140.437},
	{dir="left", pos=140.842},
	{dir="right", pos=141.29},
	{dir="left", pos=141.759},
	{dir="right", pos=142.165},
	{dir="up", pos=142.57},
	{dir="down", pos=142.975},
	{dir="up", pos=143.423},
	{dir="down", pos=143.786},
	{dir="up", pos=147.733},
	{dir="down", pos=148.159},
	{dir="left", pos=148.586},
	{dir="right", pos=148.97},
	{dir="up", pos=149.439},
	{dir="down", pos=149.823},
	{dir="left", pos=150.293},
	{dir="left", pos=150.57},
	{dir="left", pos=150.805},
	{dir="left", pos=150.911},
	{dir="left", pos=151.082},
	{dir="left", pos=151.21},
	{dir="up", pos=151.658},
	{dir="down", pos=152.021},
	{dir="up", pos=152.49},
	{dir="down", pos=152.831},
	{dir="right", pos=153.279},
	{dir="up", pos=153.685},
	{dir="right", pos=154.154},
	{dir="up", pos=154.581},
	{dir="left", pos=155.007},
	{dir="down", pos=155.434},
	{dir="left", pos=155.861},
	{dir="up", pos=156.287},
	{dir="down", pos=156.714},
	{dir="right", pos=157.119},
	{dir="up", pos=157.589},
	{dir="left", pos=157.994},
	{dir="down", pos=158.399},
	{dir="down", pos=158.805},
	{dir="down", pos=159.274},
	{dir="left", pos=159.722},
	{dir="right", pos=160.149},
	{dir="up", pos=160.575},
	{dir="up", pos=161.002},
	{dir="up", pos=161.471},
	{dir="down", pos=161.898},
	{dir="left", pos=162.261},
	{dir="right", pos=162.709},
	{dir="up", pos=163.093},
	{dir="up", pos=163.583},
	{dir="down", pos=164.031},
	{dir="left", pos=164.415},
	{dir="right", pos=164.885},
	{dir="up", pos=166.421},
	{dir="up", pos=166.634},
	{dir="up", pos=166.954},
	{dir="up", pos=167.253},
	{dir="up", pos=167.615},
	{dir="up", pos=167.957},
	{dir="down", pos=169.834},
	{dir="down", pos=170.069},
	{dir="down", pos=170.41},
	{dir="down", pos=170.709},
	{dir="down", pos=171.05},
	{dir="down", pos=171.349},
	{dir="right", pos=171.754},
	{dir="right", pos=172.159},
	{dir="right", pos=172.629},
	{dir="right", pos=173.055},
	{dir="right", pos=173.482},
	{dir="right", pos=173.887},
	{dir="right", pos=174.335},
	{dir="right", pos=174.762},
	{dir="right", pos=175.21},
	{dir="right", pos=175.615},
	{dir="right", pos=176.021},
	{dir="right", pos=176.426},
	{dir="right", pos=176.874},
	{dir="right", pos=177.279},
	{dir="left", pos=177.791},
	{dir="left", pos=177.962},
	{dir="left", pos=178.239},
	{dir="left", pos=178.41},
	{dir="left", pos=178.538},
	{dir="up", pos=178.73},
	{dir="down", pos=179.029},
	{dir="up", pos=179.413},
	{dir="down", pos=179.818},
	{dir="left", pos=180.181},
	{dir="up", pos=180.714},
	{dir="right", pos=181.162},
	{dir="down", pos=181.567},
	{dir="left", pos=181.951},
	{dir="right", pos=182.421},
	{dir="up", pos=182.847},
	{dir="down", pos=183.253},
	{dir="down", pos=183.679},
	{dir="right", pos=184.149},
	{dir="up", pos=184.554},
	{dir="left", pos=184.938},
	{dir="down", pos=185.365},
	{dir="up", pos=185.706},
	{dir="left", pos=186.239},
	{dir="right", pos=186.73},
	{dir="right", pos=187.093},
	{dir="up", pos=187.541},
	{dir="down", pos=188.01},
	{dir="left", pos=188.415},
	{dir="up", pos=188.863},
	{dir="down", pos=189.247},
	{dir="up", pos=189.717},
	{dir="down", pos=190.101},
	{dir="left", pos=190.549},
	{dir="left", pos=190.975},
	{dir="right", pos=191.423},
	{dir="up", pos=191.85},
	{dir="down", pos=192.277},
	{dir="down", pos=192.426},
	{dir="down", pos=192.597},
	{dir="down", pos=192.767},
	{dir="down", pos=192.938},
	{dir="down", pos=193.087},
	{dir="down", pos=193.258},
	{dir="down", pos=193.45},
	{dir="down", pos=193.599},
	{dir="down", pos=193.749},
	{dir="down", pos=193.919},
	{dir="down", pos=194.069},
	{dir="up", pos=194.858},
	{dir="up", pos=195.199},
	{dir="up", pos=195.498},
	{dir="right", pos=195.711},
	{dir="right", pos=195.861},
	{dir="right", pos=196.01},
	{dir="right", pos=196.159},
	{dir="right", pos=196.33},
	{dir="right", pos=196.479},
	{dir="right", pos=196.65},
	{dir="right", pos=196.842},
	{dir="right", pos=196.991},
	{dir="right", pos=197.183},
	{dir="right", pos=197.375},
	{dir="right", pos=197.525},
	{dir="left", pos=198.314},
	{dir="left", pos=198.655},
	{dir="left", pos=198.975},
	{dir="up", pos=199.189},
	{dir="up", pos=199.338},
	{dir="up", pos=199.487},
	{dir="up", pos=199.658},
	{dir="up", pos=199.829},
	{dir="up", pos=199.957},
	{dir="up", pos=200.149},
	{dir="up", pos=200.319},
	{dir="up", pos=200.49},
	{dir="up", pos=200.639},
	{dir="down", pos=200.895},
	{dir="up", pos=201.749},
	{dir="up", pos=202.09},
	{dir="up", pos=202.346},
	{dir="right", pos=202.559},
	{dir="left", pos=202.965},
	{dir="up", pos=203.434},
	{dir="down", pos=203.839},
	{dir="right", pos=204.287},
	{dir="up", pos=204.607},
	{dir="left", pos=204.927},
	{dir="down", pos=205.247},
	{dir="right", pos=205.589},
	{dir="up", pos=205.823},
	{dir="left", pos=206.058},
	{dir="down", pos=206.378},
	{dir="right", pos=206.655},
	{dir="up", pos=206.954},
	{dir="left", pos=207.253},
	{dir="down", pos=207.509},
	{dir="right", pos=207.722},
	{dir="up", pos=208.106},
	{dir="left", pos=208.362},
	{dir="down", pos=208.639},
	{dir="right", pos=208.959},
	{dir="up", pos=209.215},
	{dir="left", pos=209.45},
	{dir="down", pos=209.77},
	{dir="up", pos=210.069},
	{dir="down", pos=210.282},
	{dir="right", pos=211.242},
	{dir="right", pos=211.37},
	{dir="right", pos=211.519},
	{dir="right", pos=211.669},
	{dir="right", pos=211.797},
	{dir="right", pos=211.946},
	{dir="right", pos=212.095},
	{dir="right", pos=212.266},
	{dir="left", pos=212.458},
	{dir="left", pos=212.629},
	{dir="left", pos=212.799},
	{dir="up", pos=213.077},
	{dir="up", pos=213.333},
	{dir="up", pos=213.503},
	{dir="down", pos=213.738},
	{dir="down", pos=214.015},
	{dir="down", pos=214.271},
	{dir="left", pos=214.506},
	{dir="up", pos=214.719},
	{dir="up", pos=214.911},
	{dir="up", pos=215.082},
	{dir="left", pos=215.253},
	{dir="left", pos=215.423},
	{dir="up", pos=215.53},
	{dir="up", pos=215.743},
	{dir="left", pos=215.978},
	{dir="down", pos=216.234},
	{dir="down", pos=216.298},
	{dir="down", pos=216.703},
	{dir="down", pos=216.917},
	{dir="right", pos=217.13},
	{dir="right", pos=217.493},
	{dir="right", pos=217.621},
	{dir="right", pos=217.791},
	{dir="up", pos=218.111},
	{dir="left", pos=218.517},
	{dir="down", pos=218.879},
	{dir="down", pos=219.071},
	{dir="down", pos=219.221},
	{dir="up", pos=219.477},
	{dir="down", pos=219.669},
	{dir="up", pos=219.839},
	{dir="down", pos=220.074},
	{dir="left", pos=220.607},
	{dir="left", pos=221.013},
	{dir="left", pos=221.29},
	{dir="up", pos=221.503},
	{dir="down", pos=221.738},
	{dir="right", pos=222.271},
	{dir="up", pos=222.399},
	{dir="left", pos=222.591},
	{dir="down", pos=222.805},
	{dir="up", pos=223.125},
	{dir="down", pos=223.509},
	{dir="down", pos=223.722},
	{dir="left", pos=223.893},
	{dir="right", pos=224.042},
	{dir="left", pos=224.874},
	{dir="left", pos=225.13},
	{dir="right", pos=225.301},
	{dir="left", pos=225.663},
	{dir="right", pos=225.962},
	{dir="left", pos=226.111},
	{dir="up", pos=226.282},
	{dir="down", pos=226.517},
	{dir="down", pos=226.773},
	{dir="down", pos=227.05},
	{dir="down", pos=227.327},
	{dir="up", pos=227.541},
	{dir="up", pos=227.797},
	{dir="up", pos=228.138},
	{dir="left", pos=228.33},
	{dir="left", pos=228.757},
	{dir="right", pos=229.162},
	{dir="right", pos=229.375},
	{dir="right", pos=229.589},
	{dir="left", pos=229.759},
	{dir="right", pos=229.994},
	{dir="up", pos=230.826},
	{dir="up", pos=231.253},
	{dir="up", pos=231.53},
	{dir="down", pos=232.191},
	{dir="left", pos=232.703},
	{dir="right", pos=233.002},
	{dir="right", pos=233.279},
	{dir="right", pos=233.493},
	{dir="right", pos=233.663},
	{dir="right", pos=233.898},
	{dir="left", pos=234.346},
	{dir="left", pos=234.581},
	{dir="left", pos=234.965},
	{dir="up", pos=235.199},
	{dir="down", pos=235.583},
	{dir="up", pos=236.095},
	{dir="left", pos=236.223},
	{dir="left", pos=236.565},
	{dir="down", pos=236.757},
	{dir="right", pos=236.949},
	{dir="up", pos=237.226},
	{dir="left", pos=237.397},
	{dir="down", pos=237.653},
	{dir="up", pos=238.57},
	{dir="up", pos=238.954},
	{dir="up", pos=239.295},
	{dir="up", pos=239.487},
	{dir="up", pos=239.85},
	{dir="up", pos=240.106},
	{dir="up", pos=240.319},
	{dir="up", pos=240.661},
	{dir="up", pos=240.959},
	{dir="up", pos=241.194},
	{dir="up", pos=241.535},
	{dir="up", pos=241.834},
	{dir="up", pos=242.047},
	{dir="up", pos=242.367},
	{dir="up", pos=242.666},
	{dir="up", pos=242.879},
	{dir="up", pos=243.199},
	{dir="up", pos=243.519},
	{dir="up", pos=243.775},
	{dir="up", pos=244.074},
	{dir="up", pos=244.373},
	{dir="up", pos=244.607},
	{dir="up", pos=244.906},
	{dir="up", pos=245.226},
	{dir="down", pos=245.482},
	{dir="down", pos=245.866},
	{dir="up", pos=245.866},
	{dir="down", pos=246.335},
	{dir="down", pos=246.719},
	{dir="down", pos=247.167},
	{dir="down", pos=247.594},
	{dir="down", pos=248.021},
	{dir="down", pos=248.426},
	{dir="right", pos=248.874},
	{dir="right", pos=249.322},
	{dir="right", pos=249.77},
	{dir="right", pos=250.175},
	{dir="right", pos=250.623},
	{dir="right", pos=250.922},
	{dir="left", pos=251.434},
	{dir="left", pos=251.647},
	{dir="left", pos=251.818},
	{dir="left", pos=251.989},
	{dir="left", pos=252.117},
	{dir="left", pos=252.266},
	{dir="up", pos=252.351},
	{dir="down", pos=252.607},
	{dir="up", pos=252.842},
	{dir="down", pos=253.098},
	{dir="up", pos=253.29},
	{dir="down", pos=253.461},
	{dir="left", pos=253.802},
	{dir="right", pos=254.165},
	{dir="up", pos=254.357},
	{dir="left", pos=254.549},
	{dir="down", pos=254.741},
	{dir="right", pos=254.933},
	{dir="up", pos=255.658},
	{dir="left", pos=255.978},
	{dir="down", pos=256.191},
	{dir="right", pos=256.447},
	{dir="up", pos=256.639},
	{dir="left", pos=256.789},
	{dir="down", pos=257.002},
	{dir="right", pos=257.194},
	{dir="up", pos=257.429},
	{dir="left", pos=257.578},
	{dir="down", pos=257.791},
	{dir="right", pos=257.962},
	{dir="up", pos=258.218},
	{dir="left", pos=258.367},
	{dir="up", pos=259.199},
	{dir="down", pos=259.37},
	{dir="up", pos=259.605},
	{dir="down", pos=259.775},
	{dir="up", pos=259.989},
	{dir="down", pos=260.159},
	{dir="right", pos=260.714},
	{dir="up", pos=261.055},
	{dir="left", pos=261.247},
	{dir="down", pos=261.439},
	{dir="right", pos=261.631},
	{dir="up", pos=261.866},
	{dir="up", pos=262.57},
	{dir="down", pos=262.783},
	{dir="up", pos=262.997},
	{dir="down", pos=263.21},
	{dir="up", pos=263.423},
	{dir="down", pos=263.594},
	{dir="up", pos=263.829},
	{dir="down", pos=264.021},
	{dir="up", pos=264.255},
	{dir="down", pos=264.426},
	{dir="up", pos=264.639},
	{dir="down", pos=264.831},
	{dir="up", pos=265.045},
	{dir="down", pos=265.215},
	{dir="left", pos=265.834},
	{dir="left", pos=266.026},
	{dir="left", pos=266.218},
	{dir="left", pos=266.431},
	{dir="left", pos=266.687},
	{dir="left", pos=266.965},
	{dir="left", pos=267.178},
	{dir="left", pos=267.349},
	{dir="left", pos=267.541},
	{dir="left", pos=267.711},
	{dir="right", pos=268.607},
	{dir="right", pos=268.885},
	{dir="right", pos=269.205},
	{dir="up", pos=269.439},
	{dir="up", pos=269.61},
	{dir="up", pos=269.781},
	{dir="up", pos=269.93},
	{dir="up", pos=270.122},
	{dir="up", pos=270.293},
	{dir="up", pos=270.506},
	{dir="up", pos=270.677},
	{dir="up", pos=270.847},
	{dir="up", pos=271.039},
	{dir="up", pos=271.21},
	{dir="down", pos=271.978},
	{dir="down", pos=272.341},
	{dir="down", pos=272.597},
	{dir="right", pos=272.831},
	{dir="right", pos=272.981},
	{dir="right", pos=273.151},
	{dir="right", pos=273.301},
	{dir="right", pos=273.471},
	{dir="right", pos=273.663},
	{dir="right", pos=273.834},
	{dir="right", pos=274.026},
	{dir="right", pos=274.197},
	{dir="right", pos=274.367},
	{dir="right", pos=274.559},
	{dir="left", pos=275.391},
	{dir="left", pos=275.754},
	{dir="left", pos=275.989},
	{dir="up", pos=276.245},
	{dir="down", pos=276.479},
	{dir="up", pos=276.735},
	{dir="down", pos=276.949},
	{dir="left", pos=277.141},
	{dir="right", pos=277.567},
	{dir="up", pos=277.973},
	{dir="left", pos=278.271},
	{dir="down", pos=278.613},
	{dir="right", pos=278.933},
	{dir="up", pos=279.253},
	{dir="right", pos=279.53},
	{dir="down", pos=279.765},
	{dir="left", pos=280.085},
	{dir="up", pos=280.362},
	{dir="right", pos=280.682},
	{dir="down", pos=280.959},
	{dir="left", pos=281.215},
	{dir="up", pos=281.45},
	{dir="down", pos=281.813},
	{dir="right", pos=282.047},
	{dir="up", pos=282.41},
	{dir="left", pos=282.709},
	{dir="down", pos=282.943},
	{dir="right", pos=283.178},
	{dir="up", pos=283.455},
	{dir="left", pos=283.711},
	{dir="down", pos=283.989},
	{dir="up", pos=211.228}
}

--Called when this node is created
function node_burgermakingcontroller:init()
	self.LAUNCH_TOP = node_get("noteLauncherTop")
	self.LAUNCH_BOTTOM = node_get("noteLauncherBottom")
	self.LAUNCH_LEFT = node_get("noteLauncherLeft")
	self.LAUNCH_RIGHT = node_get("noteLauncherRight")
	self.RECEIVE_TOP = node_get("noteReceiverTop")
	self.RECEIVE_BOTTOM = node_get("noteReceiverBottom")
	self.RECEIVE_LEFT = node_get("noteReceiverLeft")
	self.RECEIVE_RIGHT = node_get("noteReceiverRight")
	self.TIME = 0
	self.NOTE_DELAY = 0.5	--Seconds between notes
	self.NEXT_NOTE = self.NOTE_DELAY
	self.NOTE_TIME = 1.5	--Seconds note is travelling for
	self.CUR_NOTE = 1		--Current note we're playing
	
	music_rewind()
end

--Called when an object enters this node
function node_burgermakingcontroller:collide(object)
end

--Called every timestep to update the node
function node_burgermakingcontroller:update(dt)

	local curMusicPos = music_getPos()
	
	if self.CUR_NOTE <= #notes then
		
		local targetLaunchNoteTime = notes[self.CUR_NOTE].pos - self.NOTE_TIME
		
		if curMusicPos >= targetLaunchNoteTime then
			--Launch this note
			print("launch "..notes[self.CUR_NOTE].dir)
			
			local launchfrom
			local receiveto
			local image
			if notes[self.CUR_NOTE].dir == "up" then
				launchfrom = self.LAUNCH_TOP
				receiveto = self.RECEIVE_TOP
				image = "res/gfx/marbleyellow.png"
			elseif notes[self.CUR_NOTE].dir == "down" then
				launchfrom = self.LAUNCH_BOTTOM
				receiveto = self.RECEIVE_BOTTOM
				image = "res/gfx/marblegreen.png"
			elseif notes[self.CUR_NOTE].dir == "left" then
				launchfrom = self.LAUNCH_LEFT
				receiveto = self.RECEIVE_LEFT
				image = "res/gfx/marbleblue.png"
			else
				launchfrom = self.LAUNCH_RIGHT
				receiveto = self.RECEIVE_RIGHT
				image = "res/gfx/marblered.png"
			end
			
			local nodex, nodey = node_getPos(launchfrom)
			local destx, desty = node_getPos(receiveto)
			local sizex, sizey = node_getVec2Property(launchfrom, "size")
			--Randomize x pos along node
			local xpos = nodex + (sizex * math.random()) - (sizex / 2.0)
			local ypos = nodey + (sizey * math.random()) - (sizey / 2.0)
			
			local actualNoteTime = notes[self.CUR_NOTE].pos - curMusicPos
			
			--Set vel to get there in the given period of time
			local xvel = (destx - xpos) / actualNoteTime
			local yvel = (desty - ypos) / actualNoteTime
			local o = obj_create("note", xpos, ypos)
			
			obj_setVelocity(o, xvel, yvel)
			obj_setImage(o, image)
			
			--Wait for next note
			self.CUR_NOTE = self.CUR_NOTE + 1
		end
		
	else
		print("End of note list: "..#notes..", "..self.CUR_NOTE)
	end
end

--Called when node is destroyed
function node_burgermakingcontroller:destroy()
end

return node_burgermakingcontroller
