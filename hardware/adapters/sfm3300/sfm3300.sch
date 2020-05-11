EESchema Schematic File Version 5
EELAYER 32 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
Comment5 ""
Comment6 ""
Comment7 ""
Comment8 ""
Comment9 ""
$EndDescr
Wire Wire Line
	4250 2800 4350 2800
Wire Wire Line
	4250 2900 4350 2900
Wire Wire Line
	4250 3000 4350 3000
Wire Wire Line
	4250 3100 4350 3100
Wire Wire Line
	4250 3200 4350 3200
Wire Wire Line
	4250 3300 4350 3300
Wire Wire Line
	5650 2750 5750 2750
Wire Wire Line
	5650 2850 5750 2850
Wire Wire Line
	5650 2950 5750 2950
Wire Wire Line
	5650 3050 5750 3050
Wire Wire Line
	5650 3150 5750 3150
Wire Wire Line
	5650 3250 5750 3250
Text GLabel 4250 2800 0    50   Input ~ 0
HEAT_GND
Text GLabel 4250 2900 0    50   Input ~ 0
VDD
Text GLabel 4250 3000 0    50   Input ~ 0
SCK
Text GLabel 4250 3100 0    50   Input ~ 0
GND
Text GLabel 4250 3200 0    50   Input ~ 0
DATA
Text GLabel 4250 3300 0    50   Input ~ 0
HEAT
Text GLabel 5650 2750 0    50   Input ~ 0
HEAT_GND
Text GLabel 5650 2850 0    50   Input ~ 0
VDD
Text GLabel 5650 2950 0    50   Input ~ 0
SCK
Text GLabel 5650 3050 0    50   Input ~ 0
GND
Text GLabel 5650 3150 0    50   Input ~ 0
DATA
Text GLabel 5650 3250 0    50   Input ~ 0
HEAT
$Comp
L Connector_Generic:Conn_01x06 Pongo1
U 1 1 5E9CC256
P 4550 3000
F 0 "Pongo1" H 4630 2992 50  0000 L CNN
F 1 "Conn_01x06" H 4630 2901 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 4550 3000 50  0001 C CNN
F 3 "~" H 4550 3000 50  0001 C CNN
	1    4550 3000
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x06 Con2.54
U 1 1 5E9D2415
P 5950 2950
F 0 "Con2.54" H 6030 2942 50  0000 L CNN
F 1 "Conn_01x06" H 6030 2851 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Horizontal" H 5950 2950 50  0001 C CNN
F 3 "~" H 5950 2950 50  0001 C CNN
	1    5950 2950
	1    0    0    -1  
$EndComp
$EndSCHEMATC
