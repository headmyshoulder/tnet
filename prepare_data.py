#! /usr/bin/python

import os
import glob
import argparse



parser = argparse.ArgumentParser( description = 'Prepare VBB data for tnet.' )
parser.add_argument( 'input_file' ,
                     metavar = 'InputFile' ,
                     type = str , 
                     help = 'Input zip file with gtfs database of the VBB schedule.' )
parser.add_argument( 'output_directory' ,
                     metavar = 'OutputDirectory' ,
                     type = str ,
                     help = 'Output directory for the gtfs database for the VBB schedule.' )
parser.add_argument( 'output_file' ,
                     metavar = 'OutputFile' ,
                     type = str ,
                     help = 'Output zip file with gtfs database of the VBB schedule.' )

args = parser.parse_args()

input_file = args.input_file
input_file_name = os.path.basename( input_file )
input_file_dir = os.path.dirname( input_file )

output_file = args.output_file
output_file_name = os.path.basename( output_file )
output_file_dir = os.path.dirname( output_file )



#def prepare_file( filename ):
    #os.system( "iconv -c -f UTF-8 -t UTF-8 " + filename + " > tmp.txt" )
    #os.system( "mv tmp.txt " + filename )


print "Creating output directory " + args.output_directory + "."
if os.path.isdir( args.output_directory ):
    print "  Already exists."
else:
    os.makedirs( args.output_directory )
    print "  Created."
    
    


print "Extracting input zip file " + input_file + "."
os.system( "cp " + input_file + " " + args.output_directory )
pwd = os.getcwd()
os.chdir( args.output_directory )
os.system( "unzip " + input_file_name )


print "Converting gtfs files in " + args.output_directory
files = glob.glob( "*.txt" )
for f in files:
    os.system( "dos2unix " + f )
    os.system( "iconv -c -f UTF-8 -t UTF-8 " + f + " > tmp.txt" )
    os.system( "mv tmp.txt " + f )


p = os.path.join( pwd , output_file_dir , output_file_name )
print "Creating zip archive " + p
os.system( "zip " + p + " *.txt" )







#prepare_file( "data/vbb_2011_2/agency.txt" )
#prepare_file( "data/vbb_2011_2/calendar_dates.txt" )
#prepare_file( "data/vbb_2011_2/calendar.txt" )
#prepare_file( "data/vbb_2011_2/routes.txt" )
#prepare_file( "data/vbb_2011_2/shapes.txt" )
#prepare_file( "data/vbb_2011_2/stops.txt" )
#prepare_file( "data/vbb_2011_2/stop_times.txt" )
#prepare_file( "data/vbb_2011_2/transfers.txt" )
#prepare_file( "data/vbb_2011_2/trips.txt" )