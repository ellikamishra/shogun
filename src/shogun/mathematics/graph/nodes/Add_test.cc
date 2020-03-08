#include <gtest/gtest.h>

#include <shogun/mathematics/UniformIntDistribution.h>
#include <shogun/mathematics/UniformRealDistribution.h>
#include <shogun/mathematics/graph/Graph.h>
#include <shogun/mathematics/graph/nodes/Add.h>
#include <shogun/mathematics/graph/nodes/Input.h>

#include "../test/GraphTest.h"

#include <random>

using namespace shogun;
using namespace shogun::graph;
using namespace std;

TYPED_TEST(GraphTest, vector_add)
{
	using NumericType = typename TypeParam::c_type;

	if constexpr (std::is_same_v<TypeParam, BooleanType>)
		return;

	auto X1 = SGVector<NumericType>(10);
	auto X2 = SGVector<NumericType>(10);

	X1.range_fill();
	X2.range_fill();

	auto expected_result1 = X1 + X2;
	auto expected_result2 = expected_result1 + X2;

	auto input = make_shared<node::Input>(
	    Shape{Shape::Dynamic}, TypeParam::type_id);
	auto input1 = make_shared<node::Input>(
	    Shape{10}, TypeParam::type_id);

	auto intermediate = input + input;

	auto output = intermediate + input1;

	auto graph = make_shared<Graph>(
	    vector{input, input1},
	    vector<shared_ptr<node::Node>>{intermediate, output});
	this->test_binary_op_results(
	    graph, X1, X2, expected_result1, expected_result2);
}

TYPED_TEST(GraphTest, vector_scalar_add)
{
	using NumericType = typename TypeParam::c_type;

	if constexpr (std::is_same_v<TypeParam, BooleanType>)
		return;

	auto X1 = SGVector<NumericType>(10);
	NumericType X2{1};

	X1.range_fill();

	auto expected_result1 = X1.clone();
	expected_result1.add(X2);

	auto input1 = make_shared<node::Input>(
	    Shape{Shape::Dynamic}, TypeParam::type_id);
	auto input2 = make_shared<node::Input>(
	    Shape{}, TypeParam::type_id);

	auto output = input1 + input2;

	auto graph = make_shared<Graph>(
	    vector{input1, input2}, vector<shared_ptr<node::Node>>{output});

	for (auto&& backend : this->m_backends)
	{
		graph->build(backend);

		std::vector<std::shared_ptr<shogun::graph::Tensor>> result =
		    graph->evaluate(
		        std::vector{std::make_shared<shogun::graph::Tensor>(X1),
		                    std::make_shared<shogun::graph::Tensor>(X2)});

		auto result1 = result[0]->as<shogun::SGVector<NumericType>>();

		for (const auto& [expected_i, result_i] :
		     shogun::zip_iterator(expected_result1, result1))
		{
			EXPECT_EQ(expected_i, result_i);
		}
	}
}

TYPED_TEST(GraphTest, scalar_vector_add)
{
	using NumericType = typename TypeParam::c_type;

	if constexpr (std::is_same_v<TypeParam, BooleanType>)
		return;

	NumericType X1{1};
	auto X2 = SGVector<NumericType>(10);
	X2.range_fill();

	auto expected_result1 = X2.clone();
	expected_result1.add(X1);

	auto input1 = make_shared<node::Input>(
	    Shape{}, TypeParam::type_id);
	auto input2 = make_shared<node::Input>(
	    Shape{Shape::Dynamic}, TypeParam::type_id);

	auto output = input1 + input2;

	auto graph = make_shared<Graph>(
	    vector{input1, input2}, vector<shared_ptr<node::Node>>{output});

	for (auto&& backend : this->m_backends)
	{
		graph->build(backend);

		std::vector<std::shared_ptr<shogun::graph::Tensor>> result =
		    graph->evaluate(
		        std::vector{std::make_shared<shogun::graph::Tensor>(X1),
		                    std::make_shared<shogun::graph::Tensor>(X2)});

		auto result1 = result[0]->as<shogun::SGVector<NumericType>>();

		for (const auto& [expected_i, result_i] :
		     shogun::zip_iterator(expected_result1, result1))
		{
			EXPECT_EQ(expected_i, result_i);
		}
	}
}

TYPED_TEST(GraphTest, matrix_add)
{
	using NumericType = typename TypeParam::c_type;

	random_device rng_device;
	mt19937_64 mersenne_engine{rng_device()};

	auto X1 = SGMatrix<NumericType>(10, 5);
	auto X2 = SGMatrix<NumericType>(10, 5);
	auto expected_result1 = SGMatrix<NumericType>(10, 5);
	auto expected_result2 = SGMatrix<NumericType>(10, 5);

	if constexpr (std::is_same_v<TypeParam, BooleanType>)
		return;
	else if constexpr (std::is_floating_point_v<NumericType>)
	{
		UniformRealDistribution<NumericType> dist;
		auto gen = [&dist, &mersenne_engine]() {
			return dist(mersenne_engine);
		};
		generate(X1.begin(), X1.end(), gen);
		generate(X2.begin(), X2.end(), gen);
	}
	else
	{
		UniformIntDistribution<NumericType> dist;
		auto gen = [&dist, &mersenne_engine]() {
			return dist(mersenne_engine);
		};
		generate(X1.begin(), X1.end(), gen);
		generate(X2.begin(), X2.end(), gen);
	}

	std::transform(
	    X1.data(), X1.data() + X1.size(), X1.data(), expected_result1.data(),
	    std::plus<NumericType>{});
	std::transform(
	    expected_result1.data(),
	    expected_result1.data() + expected_result1.size(), X2.data(),
	    expected_result2.data(), std::plus<NumericType>{});

	auto input = make_shared<node::Input>(
	    Shape{Shape::Dynamic, 5}, TypeParam::type_id);
	auto input1 = make_shared<node::Input>(
	    Shape{10, Shape::Dynamic}, TypeParam::type_id);

	auto intermediate = input + input;

	auto output = intermediate + input1;

	auto graph = make_shared<Graph>(
	    vector{input, input1},
	    vector<shared_ptr<node::Node>>{intermediate, output});

	for (auto&& backend : this->m_backends)
	{
		graph->build(backend);

		vector<shared_ptr<Tensor>> result = graph->evaluate(
		    vector{make_shared<Tensor>(X1), make_shared<Tensor>(X2)});

		auto result1 = result[0]->as<SGMatrix<NumericType>>();
		auto result2 = result[1]->as<SGMatrix<NumericType>>();

		for (const auto& [expected_i, result_i] :
		     zip_iterator(expected_result1, result1))
		{
			EXPECT_EQ(expected_i, result_i);
		}

		for (const auto& [expected_i, result_i] :
		     zip_iterator(expected_result2, result2))
		{
			EXPECT_EQ(expected_i, result_i);
		}
	}
}
