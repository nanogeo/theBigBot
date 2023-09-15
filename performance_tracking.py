import csv
import numpy

def read_file(file_name):
	frame_time = open('F:/TossBotCpp - Copy/TossBotCpp/build/src/' + file_name, 'r')

	frame_time_csv = csv.reader(frame_time)

	elements = []
	element_counts = []
	categories = []

	first_row = True

	for row in frame_time_csv:
		if first_row:
			elements = numpy.zeros(len(row), int)
			element_counts = numpy.zeros(len(row), int)
			first_row = False
			categories = row
			continue
		element_index = 0
		for element in row:
			elements[element_index] = elements[element_index] + int(element)
			element_counts[element_index] = element_counts[element_index] + 1
			element_index = element_index + 1

	return (categories, elements, element_counts)

def display_times(info, indent):
	total = 0
	for i in range(0, len(info[0])):
		avg = 0
		if info[2][i] != 0:
			avg = info[1][i] / info[2][i]
		
		total = total + avg
		print('   |' * indent + '{:<25} {:>8}'.format(info[0][i], "{:.2f}".format(avg)))
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

	print('   |' * indent + '{:<25} {:>8}'.format('Total', "{:.2f}".format(total)))

frame_time = read_file('frame_time.txt')
print('\nFrame Time')
display_times(frame_time, 0)
