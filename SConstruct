env = Environment(CPPPATH = 'include')

components = ['repl.o', 'solver.o', 'input.o', 'stack.o', 'numbers.o']
components_source = ['repl.c', 'solver.c', 'input.c', 'stack.c', 'numbers.c']

progname = 'longcalc'
progsource = ['main.c'] + components_source

env.Program(target = progname, source = progsource)