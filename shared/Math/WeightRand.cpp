#include "PlatformPrecomp.h"
#include "WeightRand.h"
#include <cassert>

CWeightRand::CWeightRand()
{
	m_b_needs_computation = true;
}

int CWeightRand::GetOdds(int i_choice)
{
	assert(i_choice >= 0 && i_choice < int(a_odds.size()) && "Invalid index!");
	return a_odds[i_choice];
}

void CWeightRand::Clear()
{
	a_odds.clear();
	a_computed_odds.clear();
	m_b_needs_computation = true;

}

bool CWeightRand::Save(FILE *fp)       
{
	//save our info out into a filestream
	int i_elements = (int)a_odds.size();
	fwrite(&i_elements, sizeof(i_elements), 1, fp);

	//next write out the elements

	for (int i=0; i < i_elements; i++)
	{
		fwrite(&a_odds[i], sizeof(int), 1, fp);
	}
	return true;
}

bool CWeightRand::Load(FILE *fp)       
{
	//save our info out into a filestream
	int i_elements = 0;

	fread(&i_elements, sizeof(i_elements), 1, fp);

	//next read in all the elements
	Clear();

	for (int i=0; i < i_elements; i++)
	{
		int i_odds;
		fread(&i_odds, sizeof(int), 1, fp);
		AddChoice(i, i_odds);
	}
	return true;
}


void CWeightRand::ModChoice(int index, int mod)
{
	AddChoice(index, GetOdds(index)+mod);
}

void CWeightRand::AddChoice(int i_index, int i_odds)
{
	if (i_index+1 > int(a_odds.size()))
	{
		a_odds.resize(i_index+1); //make sure it's big enough
		a_computed_odds.resize(i_index+1); //make sure it's big enough
	}

	a_odds[i_index] = i_odds;
	m_b_needs_computation = true;
}


void CWeightRand::ComputeOdds()
{
	//cycle through and figure out the percentages

	long l_total = 0;    
	int i;

	for (i=0; i < int(a_odds.size()); i++)
	{
		l_total += a_odds[i];
	}

	for (i=0; i < int(a_odds.size()); i++)
	{
		a_computed_odds[i] = float(a_odds[i])/ float(l_total);
		float test = a_computed_odds[i];
	}


	m_b_needs_computation = false; //all done, ready to compute crap

}

int CWeightRand::CalcNumber(float f_rand)
{
	//scroll through and figure out which thing we chose

	float f_temp = 0;

	for (int i=0; i < int(a_computed_odds.size()); i++)
	{
		f_temp += a_computed_odds[i];
		if (f_temp > f_rand)
		{
			//this must be it.
			return i;
		}
	}

	return 0;
}

int CWeightRand::GetRandom(CRandom &rng)
{

	if (m_b_needs_computation) this->ComputeOdds();

	float f_rand = ((float)rng.rand() / RT_RAND_MAX);

	return CalcNumber(f_rand);
}

int CWeightRand::GetRandom()
{

	if (m_b_needs_computation) this->ComputeOdds();

	float f_rand = ((float)rand() / RT_RAND_MAX);

	return CalcNumber(f_rand);
}


