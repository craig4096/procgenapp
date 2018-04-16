
#pragma once
#include <vector>
#include <map>
#include "math3d.h"
#include "ProbabilityFunction.h"
#include "ShapeDatabase.h"

/*
This class represents a basic shape grammar algorithm
context-free, uses set grammars, Bounding box occlusion tests and some basic shape rules
*/
class BasicShapeGrammar {
public:

    enum RHSFilterType {
        Default,
        RandSel,
        RandSelOne
    };

	struct Rule {
        RHSFilterType filterType;
		std::string lhsSymbol;
		struct RHS {
            bool isClass;   // determines whether the symbol member represents an actual shape or a class
			std::string symbol;
			vec3 translation;
			float yRotation;
		};
		std::vector<RHS> rhs;
        //float probability;
        ProbabilityFunction* probFunc;
	};

private:

    std::map<std::string, std::vector<std::string> > shapeClasses;

    static std::vector<Rule::RHS> selectRHS(const Rule& rule);

    // rules defining this shape grammar
    std::vector<Rule> rules;

	// applies a rule to a shape and returns a new set of shapes
    std::vector<Shape> applyRule(const Shape& shape, const Rule& rule, const SymbolMeshMap& symbolMeshMap);

    // determines whether the rule can be applied to the specific shape or not
    bool isCandidateRule(const ShapeDatabase& shapes, const Shape& shape,
                    const Rule& rule);


	// given an array of rules this function will randomly select a rule based on its
    // probability distribution (function)
    static int selectRule(const Shape& shape, const std::vector<Rule>& candidateRules);

public:


	BasicShapeGrammar(const std::string& rulesFile);
	~BasicShapeGrammar();

	// generates a set of shapes using this grammar. This function uses the symbol mesh map
	// only to perform an overlap test between the boundaries of the meshes. It assumes that this
	// SymbolMeshMap will also be used in rendering the final result
    void generate(ShapeDatabase& shapes, int numIterations);
};
