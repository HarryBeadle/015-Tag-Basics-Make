min_freq = 244
magic = 15925005

with open('notes-frequency.txt') as notes:
    for line in notes.read().split('\n'):
        note, frequency = line.split(' ')
        frequency = float(frequency);
        period_counter = int(magic/frequency)
        if frequency > min_freq:
            print(note, period_counter)
