/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 1999-2009 Soeren Sonnenburg
 * Copyright (C) 1999-2009 Fraunhofer Institute FIRST and Max-Planck-Society
 */

#include "preprocessor/NormOne.h"
#include "preprocessor/SimplePreprocessor.h"
#include "lib/Mathematics.h"
#include "features/Features.h"
#include "features/SimpleFeatures.h"

using namespace shogun;

CNormOne::CNormOne()
: CSimplePreprocessor<float64_t>()
{
}

CNormOne::~CNormOne()
{
}

/// initialize preprocessor from features
bool CNormOne::init(CFeatures* features)
{
	ASSERT(features->get_feature_class()==C_SIMPLE);
	ASSERT(features->get_feature_type()==F_DREAL);

	return true;
}

/// clean up allocated memory
void CNormOne::cleanup()
{
}

/// initialize preprocessor from file
bool CNormOne::load(FILE* f)
{
	SG_SET_LOCALE_C;
	SG_RESET_LOCALE;
	return false;
}

/// save preprocessor init-data to file
bool CNormOne::save(FILE* f)
{
	SG_SET_LOCALE_C;
	SG_RESET_LOCALE;
	return false;
}

/// apply preproc on feature matrix
/// result in feature matrix
/// return pointer to feature_matrix, i.e. f->get_feature_matrix();
SGMatrix<float64_t> CNormOne::apply_to_feature_matrix(CFeatures* features)
{
	SGMatrix<float64_t> feature_matrix=((CSimpleFeatures<float64_t>*)features)->get_feature_matrix();

	for (int32_t i=0; i<feature_matrix.num_cols; i++)
	{
		float64_t* vec= &(feature_matrix.matrix[i*feature_matrix.num_rows]);
		float64_t norm=CMath::sqrt(CMath::dot(vec, vec, feature_matrix.num_rows));
		CMath::scale_vector(1.0/norm, vec, feature_matrix.num_rows);
	}
	return feature_matrix;
}

/// apply preproc on single feature vector
/// result in feature matrix
SGVector<float64_t> CNormOne::apply_to_feature_vector(SGVector<float64_t> vector)
{
	float64_t* normed_vec = new float64_t[vector.length];
	float64_t norm=CMath::sqrt(CMath::dot(vector.vector, vector.vector, vector.length));

	for (int32_t i=0; i<vector.length; i++)
		normed_vec[i]=vector.vector[i]/norm;

	return SGVector<float64_t>(normed_vec,vector.length);
}
