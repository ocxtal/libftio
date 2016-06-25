#! /usr/bin/env python
# encoding: utf-8

def options(opt):
	opt.load('compiler_c')

def configure(conf):
	conf.load('ar')
	conf.load('compiler_c')

	conf.env.append_value('CFLAGS', '-O3')
	conf.env.append_value('CFLAGS', '-std=c99')
	conf.env.append_value('CFLAGS', '-march=native')

	if 'LIB_FTDI1' not in conf.env:
		conf.check_cc(lib = 'ftdi1')

	conf.env.append_value('LIB_FTIO', conf.env.LIB_FTDI1)

def build(bld):
	bld.stlib(
		source = ['ftio.c'],
		target = 'ftio',
		lib = bld.env.LIB_FTIO)

	bld.program(
		source = ['test.c'],
		target = 'test',
		use = ['ftio'],
		lib = bld.env.LIB_FTIO)
