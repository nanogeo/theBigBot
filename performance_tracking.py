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

	first_row = True
	element_list = []

	for row in frame_time_csv:
		if first_row:
			elements = numpy.zeros(len(row), int)
			element_counts = numpy.zeros(len(row), int)
			element_high_percentile = numpy.zeros(len(row), int)
			element_low_percentile = numpy.zeros(len(row), int)
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
		element_high_percentile[element_index] = high
		element_low_percentile[element_index] = low
		element_index = element_index + 1



	return (categories, elements, element_counts, element_low_percentile, element_high_percentile)

def display_times(info, indent):
	total = 0
	for i in range(0, len(info[0])):
		avg = 0
		if info[2][i] != 0:
			avg = info[1][i] / info[2][i]
		
		total = total + avg
		print('   |' * indent + '{:<25} {:>8} {:>8} {:>8}'.format(info[0][i], "{:.0f}".format(avg), "{:.0f}".format(info[3][i]), "{:.0f}".format(info[4][i])))
		if info[0][i] == 'Process actions':
			action_time = read_file('action_time.txt')
			#print('   |' * (indent + 1) + 'Action Time')
			display_times(action_time, indent + 1)
		elif info[0][i] == 'ApplyPressure':
			pressure_time = read_file('pressure_time.txt')
			#print('   |' * (indent + 1) + 'Pressure Time')
			display_times(pressure_time, indent + 1)
		elif info[0][i] == 'Find targets':
			find_targets = read_file('find_targets.txt')
			#print('   |' * (indent + 1) + 'Find Targets Time')
			display_times(find_targets, indent + 1)
			fire_control = read_file('fire_control_time.txt')
			print('   |' * (indent + 1) + 'Fire Control Time')
			display_times(fire_control, indent + 1)
		elif info[0][i] == 'Display debug':
			debug_hud = read_file('debug_hud_time.txt')
			#print('   |' * (indent + 1) + 'Pressure Time')
			display_times(debug_hud, indent + 1)

	print('   |' * indent + '{:<25} {:>8}'.format('Total', "{:.0f}".format(total)))

frame_time = read_file('frame_time.txt')
print('\nFrame Time')
display_times(frame_time, 0)
