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

def display_times(info):
	for i in range(0, len(info[0])):
		print('{0:<20} {1:>6}'.format(info[0][i], "{:.2f}".format(info[1][i] / info[2][i])))

frame_time = read_file('frame_time.txt')
print('\nFrame Time')
display_times(frame_time)

find_targets = read_file('find_targets.txt')
print('\nFind Targets Time')
display_times(find_targets)

fire_control = read_file('fire_control.txt')
print('\nFire Control Time')
display_times(fire_control)

pressure_time = read_file('pressure_time.txt')
print('\nPressure Time')
display_times(pressure_time)

action_time = read_file('action_time.txt')
print('\nAction Time')
display_times(action_time)