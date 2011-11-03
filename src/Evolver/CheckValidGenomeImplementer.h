/*******************************************************************************
 * CheckValidGenomeImplementer.h
 * 
 * Peter Fine - EASy MSC, Adaptive Systems Coursework
 * Created Nov' 07
 * 
 ******************************************************************************/

#ifndef CheckValidGenomeImplementer_h
#define CheckValidGenomeImplementer_h

class Genotype;

/**
 * The purpose of this is to allow a class to be able to check a valid genome.
 * It is abstract.
 */
class CheckValidGenomeImplementer {

    public :
    	virtual bool checkGenomeIsValid(Genotype& aGenotype) = 0;
};

#endif // CheckValidGenomeImplementer_h

