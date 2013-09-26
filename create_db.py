#! /usr/bin/python

import gtfs
import argparse

parser = argparse.ArgumentParser( description = 'Create SQLite database for the VBB data for tnet.' )
parser.add_argument( 'input_file' ,
                     metavar = 'InputFile' ,
                     type = str , 
                     help = 'Input zip file with gtfs database of the VBB schedule.' )
parser.add_argument( 'database' ,
                     metavar = 'Database' ,
                     type = str ,
                     help = 'Filename of the SQLite database for the VBB schedule.' )

args = parser.parse_args()

sched = gtfs.load( args.input_file , args.database )