#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <stack>
#include <vector>
#include <hash_map>
#include <map>
#include <set>
#include <omp.h>
#include <time.h>

//#include "switch.h"
#include "read_block_test.h"
#include "bwtmap_info.h"
#include "DoubleAnchorScore.h"
#include "sbndm.h"
#include "otherFunc.h"
#include "index_info.h"
#include "constantDefinitions.h"
#include "segmentMapping.h"
//#include "segmentMapping_secondLevel.h"
#include "splice_info.h"
#include "fixGapRelationParameters.h"
#include "read_info.h"
#include "seg_info.h"
#include "gap_info.h"
#include "align_info.h"
#include "spliceJunction_info.h"
#include "unmapEnd_info.h"
#include "unfixedHead.h"
#include "unfixedTail.h"
#include "sam2junc.h"
#include "detectFusionJunction.h"

#define PreIndexSize 268435456

using namespace std;  

int main(int argc, char**argv)
{
    if(argc < 3)
	{
		//cout << "Executable <InputReads> <InputReads_PE> <OutputSAM> <threads_num> <Fasta_or_Fastq> <HumanOrMouse>" << endl;
		cout << "Executable <InputRecords> <outputDir> <indexFilePrefix>" << endl;

		exit(0); 
	}
	string inputRecordsStr = argv[1];

	string indexStr = argv[3];

    string outputDirStr = argv[2];

   	string mkdirOutputCommand = "mkdir -p " + outputDirStr;
   	system(mkdirOutputCommand.c_str());

   	string processLogStr = outputDirStr + "/process.log";
   	ofstream log_ofs(processLogStr.c_str());

    log_ofs << "inputRecordsStr: " << inputRecordsStr << endl;
    log_ofs << "outputDir: " << outputDirStr << endl;
    log_ofs << "indexStr: " << indexStr << endl;
   	

   	indexStr += "/";
	string chrom_bit_file = indexStr; chrom_bit_file.append("_chrom"); ifstream chrom_bit_file_ifs(chrom_bit_file.c_str(),ios::binary);

	string parameter_file = indexStr; parameter_file.append("_parameter"); ifstream parameter_file_ifs(parameter_file.c_str(),ios::binary);

	Index_Info* indexInfo = new Index_Info(parameter_file_ifs);
	//log_ofs << "index: " << indexStr << endl;
	///////////////////////////////////////
 
	log_ofs << "start to load whole genome" << endl;
	char *chrom;

	chrom = (char*)malloc((indexInfo->indexSize) * sizeof(char));
	chrom_bit_file_ifs.read((char*)chrom, (indexInfo->indexSize) * sizeof(char)); 

	indexInfo->chromString = chrom;
	log_ofs << "chromSize = " <<(indexInfo->chromString).size() << endl;
	
	log_ofs << "start to load every chromosome" << endl;
	//chromStr[0] = 
	(indexInfo->chromStr).push_back((indexInfo->chromString).substr(0, (indexInfo->chrEndPosInGenome)[0]+1));
	(indexInfo->chromLength).push_back(((indexInfo->chrEndPosInGenome)[0]+1));
	for(int tmp = 1; tmp < indexInfo->chromNum; tmp++)
	{
		//chromStr[tmp] = 
		(indexInfo->chromStr).push_back((indexInfo->chromString).substr((indexInfo->chrEndPosInGenome)[tmp-1]+2, 
			(indexInfo->chrEndPosInGenome)[tmp]-(indexInfo->chrEndPosInGenome)[tmp-1]-1));	
		(indexInfo->chromLength).push_back(((indexInfo->chrEndPosInGenome)[tmp]-(indexInfo->chrEndPosInGenome)[tmp-1]-1));
	}	
	log_ofs << "finish loading all index" << endl;


	Fusion_Forw_Info* fusionInfo_forw = new Fusion_Forw_Info();

	fusionInfo_forw->generateCandidateFusionMap(inputRecordsStr, indexInfo);

	log_ofs << "fusion info: \n size: " << (fusionInfo_forw->fusionCandidate).size() << endl;
	cout << fusionInfo_forw->getFusionCandidateStr() << endl;

	log_ofs << "start to generate 2 fusionEndSets " << endl;
	fusionInfo_forw->generateCandidateFusionSet();
	log_ofs << "finish generating 2 fusionEndSets " << endl;

	cout << fusionInfo_forw->getFusionSetStr() << endl;

	return 0;
}