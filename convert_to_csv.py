

import pathlib
import csv


def parse_mem_str(string):
    value = float(string.split('):')[-1].strip())
    if string.startswith('(kbytes)'):
        factor = 1.
    elif string.startswith('(mbytes)'):
        factor = 1024.
    elif string.startswith('(bytes)'):
        factor = 1./1024
    elif string.startswith('(gbytes)'):
        factor = 1024.*1024
    else:
        raise ValueError(f'Unknown mem unit ' + string)
    return value*factor

def parse_duration_str(string):
    value, _, unit = string.strip().partition(' ')
    if unit.strip() == 'ns':
        factor = 1. / 1000.
    elif unit.strip() == 'ms':
        factor = 1.
    else:
        raise ValueError(f'Unknown time unit ' + unit)
    return float(value)*factor


def parse_experiment_result(txt_file):
    experiments = []
    with pathlib.Path(txt_file).open() as stream:
        current_experiment = None
        for line in stream.readlines():
            if line.startswith('>>>>>'):
                if current_experiment is not None:
                    experiments.append(current_experiment)
                current_experiment = {}
            elif line.startswith('> Method: '):
                current_experiment['method'] = line.replace('> Method: ', '').strip()
            elif line.startswith('> Total Count: '):
                current_experiment['hits'] = int(line.replace('> Total Count: ', '').strip())
            elif line.startswith('> Query File: '):
                current_experiment['query_file'] = line.replace('> Query File: ', '').strip().lstrip('"').rstrip('"')
                current_experiment['query_file'] = pathlib.Path(current_experiment['query_file'])      
            elif line.startswith('> Query Limit: '):
                current_experiment['query_limit'] = int(line.replace('> Query Limit: ', '').lstrip('"').rstrip('"'))
            elif line.startswith('> Search duration: '):
                current_experiment['query_time_ms'] = parse_duration_str(line.replace('> Search duration: ', '').strip())
            elif line.strip().startswith('Maximum resident set size '):
                current_experiment['mem_peak_kbytes'] = parse_mem_str(line.replace('Maximum resident set size ', '').strip())
    if current_experiment is not None and len(current_experiment) > 0:
        experiments.append(current_experiment)
    return experiments

if __name__ == '__main__':
    import sys
    experiments_txt = pathlib.Path(sys.argv[1])
    experiments = parse_experiment_result(experiments_txt)
    keys = experiments[0].keys()
    result_csv = experiments_txt.parent / (experiments_txt.stem + '.csv')

    with result_csv.open('w', newline='') as output_file:
        dict_writer = csv.DictWriter(output_file, keys)
        dict_writer.writeheader()
        dict_writer.writerows(experiments)




