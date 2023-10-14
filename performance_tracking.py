import csv
import numpy

def read_file(file_name):
	frame_time = open('F:/TossBotCpp - Copy/TossBotCpp/build/src/' + file_name, 'r')

	frame_time_csv = csv.reader(frame_time)

	elements = []
	element_counts = []
	categories = []
	element_high_percentile = []
	element_low_percentile = []
	element_very_high_percentile = []
	element_highest = []

	first_row = True
	element_list = []

	for row in frame_time_csv:
		if first_row:
			elements = numpy.zeros(len(row), int)
			element_counts = numpy.zeros(len(row), int)
			element_high_percentile = numpy.zeros(len(row), int)
			element_low_percentile = numpy.zeros(len(row), int)
			element_very_high_percentile = numpy.zeros(len(row), int)
			element_highest = numpy.zeros(len(row), int)
			element_list = [[] for i in range(len(row))]
			first_row = False
			categories = row
			continue
		element_index = 0
		for element in row:
			element_list[element_index].append(int(element))
			elements[element_index] = elements[element_index] + int(element)
			element_counts[element_index] = element_counts[element_index] + 1
			element_index = element_index + 1
			
	element_index = 0
	for row in element_list:
		row.sort()
		high_index = int(len(row) * .95)
		high = row[high_index]
		low_index = int(len(row) * .05)
		low = row[low_index]
		very_high_index = int(len(row) * .999)
		very_high = row[very_high_index]
		highest_index = int(len(row) - 1)
		highest = row[highest_index]
		element_high_percentile[element_index] = high
		element_low_percentile[element_index] = low
		element_very_high_percentile[element_index] = very_high
		element_highest[element_index] = highest
		element_index = element_index + 1



	return (categories, elements, element_counts, element_low_percentile, element_high_percentile, element_very_high_percentile, element_highest)

def display_times(info, indent):
	total = 0
	for i in range(0, len(info[0])):
		avg = 0
		if info[2][i] != 0:
			avg = info[1][i] / info[2][i]
		
		total = total + avg
		print('   |' * indent + '{:<25} {:>8} {:>8} {:>8} {:>8} {:>8}'.format(info[0][i], "{:.0f}".format(avg), "{:.0f}".format(info[3][i]), "{:.0f}".format(info[4][i]), "{:.0f}".format(info[5][i]), "{:.0f}".format(info[6][i])))
		if info[0][i] == 'Process actions':
			action_time = read_file('action_time.txt')
			#print('   |' * (indent + 1) + 'Action Time')
			display_times(action_time, indent + 1)
		if info[0][i] == 'Apply pressure':
			pressure_time = read_file('pressure_time.txt')
			#print('   |' * (indent + 1) + 'Pressure Time')
			display_times(pressure_time, indent + 1)
		if info[0][i] == 'Find targets':
			find_targets = read_file('find_targets.txt')
			#print('   |' * (indent + 1) + 'Find Targets Time')
			display_times(find_targets, indent + 1)
			fire_control = read_file('fire_control_time.txt')
			print('   |' * (indent + 1) + 'Fire Control Time')
			display_times(fire_control, indent + 1)
		if info[0][i] == 'Display debug':
			debug_hud = read_file('debug_hud_time.txt')
			#print('   |' * (indent + 1) + 'Pressure Time')
			display_times(debug_hud, indent + 1)
		if info[0][i] == 'Update enemy weapon cd':
			enemy_weapon_cd = read_file('update_weapon_cd_time.txt')
			#print('   |' * (indent + 1) + 'Pressure Time')
			display_times(enemy_weapon_cd, indent + 1)
		if info[0][i] == 'Process actions':
			make_workers = read_file('make_workers.txt')
			#print('   |' * (indent + 1) + 'Action Time')
			display_times(make_workers, indent + 1)
		if info[0][i] == 'Process actions':
			build_pylons = read_file('build_pylons.txt')
			#print('   |' * (indent + 1) + 'Action Time')
			display_times(build_pylons, indent + 1)
		if info[0][i] == 'Process actions':
			get_upgrades = read_file('get_upgrades.txt')
			#print('   |' * (indent + 1) + 'Action Time')
			display_times(get_upgrades, indent + 1)
		if info[0][i] == 'Process actions':
			chronoing = read_file('chronoing.txt')
			#print('   |' * (indent + 1) + 'Action Time')
			display_times(chronoing, indent + 1)
		if info[0][i] == 'Process actions':
			expanding = read_file('expanding.txt')
			#print('   |' * (indent + 1) + 'Action Time')
			display_times(expanding, indent + 1)
		"""if info[0][i] == 'Process actions':
			constant_chrono = read_file('constant_chrono.txt')
			#print('   |' * (indent + 1) + 'Action Time')
			display_times(constant_chrono, indent + 1)"""
		if info[0][i] == 'Process actions':
			zealot_warp = read_file('zealot_warp.txt')
			#print('   |' * (indent + 1) + 'Action Time')
			display_times(zealot_warp, indent + 1)
		if info[0][i] == 'Z - All gates ready':
			wind_spots = read_file('wind_spots.txt')
			#print('   |' * (indent + 1) + 'Action Time')
			display_times(wind_spots, indent + 1)
		if info[0][i] == 'Aiming at':
			aiming_at = read_file('amaing_at_time.txt')
			#print('   |' * (indent + 1) + 'Pressure Time')
			display_times(aiming_at, indent + 1)
		if info[0][i] == 'Update warpgate status':
			update_warpgates = read_file('update_warpgates.txt')
			#print('   |' * (indent + 1) + 'Pressure Time')
			display_times(update_warpgates, indent + 1)

	print('   |' * indent + '{:<25} {:>8}'.format('Total', "{:.0f}".format(total)))

frame_time = read_file('frame_time.txt')
print('\nFrame Time')
display_times(frame_time, 0)
