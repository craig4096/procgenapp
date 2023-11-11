
#include "BasicShapeGrammar.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <algorithm>

#include "ConstProbFunc.h"
#include "RadialProbFunc.h"
using namespace std;

BasicShapeGrammar::BasicShapeGrammar(const string& filename)
{
    ifstream ifs(filename.c_str());
    if (!ifs.is_open())
    {
        throw std::logic_error("could not open grammar file");
    }

    // load the shape rules
    string token;
    while (!ifs.eof())
    {
        ifs >> token;

        if (token == "class")
        {
            // load a class of shapes
            std::string className;
            ifs >> className;
            ifs >> token;
            if (token != "{")
            {
                throw std::logic_error("expected { in class declaration");
            }
            std::vector<std::string> shapeNames;
            while (true)
            {
                ifs >> token;
                if (token == "}")
                {
                    break;
                }
                shapeNames.push_back(token);
            }
            shapeClasses[className] = shapeNames;
            cout << "loaded class of size: " << shapeNames.size() << endl;
        }
        else
        {
            Rule rule;
            rule.lhsSymbol = token;
            if (rule.lhsSymbol.empty())
            {
                break;
            }
            ifs >> token;
            rule.filterType = Default;
            if (token == "randsel")
            {
                rule.filterType = RandSel;
                ifs >> token;
            }
            else if (token == "randselone")
            {
                rule.filterType = RandSelOne;
                ifs >> token;
            }
            if (token != "{")
            {
                throw std::logic_error("expected { got: " + token + " in " + filename);
            }
            while (true)
            {
                Rule::RHS rhs;
                ifs >> rhs.symbol;
                rhs.isClass = false;
                if (rhs.symbol == "class")
                {
                    rhs.isClass = true;
                    ifs >> rhs.symbol;
                }
                ifs >> rhs.translation.x;
                ifs >> rhs.translation.y;
                ifs >> rhs.translation.z;
                ifs >> rhs.yRotation;
                rule.rhs.push_back(rhs);
                ifs >> token;
                if (token == "}")
                {
                    break;
                }
                else if (token != "|")
                {
                    throw std::logic_error("expected |");
                }
            }
            // load in the probability function
            std::string probname;
            ifs >> probname;
            if (probname == "const")
            {
                float value;
                ifs >> value;
                rule.probFunc = new ConstProbFunc(value);
            }
            else if (probname == "radial")
            {
                vec3 pos;
                float minProb, maxProb, radius;
                ifs >> pos.x >> pos.y >> pos.z >> radius >> minProb >> maxProb;
                rule.probFunc = new RadialProbFunc(pos, minProb, maxProb, radius);
            }
            //ifs >> rule.probability;
            rules.push_back(rule);

        }
        token.clear();
    }
}

BasicShapeGrammar::~BasicShapeGrammar()
{
    // delete all probability functions of the rules
    for (int i = 0; i < rules.size(); ++i)
    {
        delete rules[i].probFunc;
    }
}

std::vector<BasicShapeGrammar::Rule::RHS> BasicShapeGrammar::selectRHS(const Rule& rule)
{
    std::vector<Rule::RHS> rhs;
    switch (rule.filterType)
    {
    case Default:
        rhs = rule.rhs;
        break;
    case RandSel:
        // assert(rule.rhs.size() > 0)
        rhs.push_back(rule.rhs[0]);
        for (int i = 1; i < rule.rhs.size(); ++i)
        {
            if (rand() % 2)
            {
                rhs.push_back(rule.rhs[i]);
            }
        }
        break;
    case RandSelOne:
        rhs.push_back(rule.rhs[(rand() % (rule.rhs.size() - 1)) + 1]);
        break;
    }
    return rhs;
}

vector<Shape> BasicShapeGrammar::applyRule(const Shape& shape, const Rule& rule, const SymbolMeshMap& symbolMeshMap)
{
    std::vector<Shape> rval;

    // select the rhs shapes based on the rule filter
    std::vector<Rule::RHS> rhss = selectRHS(rule);
    // for all rhs of the rule
    for (int i = 0; i < rhss.size(); ++i)
    {
        const Rule::RHS& rhs = rhss[i];
        Shape s;
        // if the rhs references a class then stochastically choose
        // a symbol from the class of shapes
        if (rhs.isClass)
        {
            map<string, vector<string> >::const_iterator shapes = shapeClasses.find(rhs.symbol);
            if (shapes == shapeClasses.end())
            {
                throw std::logic_error("could not find class " + rhs.symbol);
            }
            s.symbol = shapes->second[rand() % shapes->second.size()];
        }
        else
        {
            s.symbol = rhs.symbol;
        }
        // check if the rhs symbol suggests a terminal shape
        s.terminal = false;
        if (s.symbol.rfind("_term") != string::npos)
        {
            s.terminal = true;
            s.symbol = s.symbol.substr(0, s.symbol.size() - 5);
        }

        // translate new shape relative to prev shapes position and rotation
        // first rotate the translation vector so that it is relative to the current shape rotation
        s.position = shape.position + RotateVectorY(rhs.translation, shape.yRotation);

        // apply relative rotation to the new shape
        s.yRotation = shape.yRotation + rhs.yRotation;

        // update the shapes bounding box given the symbolMeshMap
        s.calculateBoundingBox(symbolMeshMap);

        rval.push_back(s);
    }
    return rval;
}

/*
bool BasicShapeGrammar::isCandidateRule(const std::vector<Shape>& currentShapes, int shapeIndex, const Rule& rule,
                        SymbolMeshMap& symbolMeshMap) {
//cout << "Testing Candidate Rule" << endl;
    const Shape& shape = currentShapes[shapeIndex];
    if(shape.symbol == rule.lhsSymbol) {
        // test for overlap
        std::vector<Shape> result = ApplyRule(shape, rule, symbolMeshMap);
        // test all rhs shapes with all currentShapes
        bool notOverlapping = true;
        for(int i = 0; i < result.size() && notOverlapping; ++i) {
            for(int j = 0; j < currentShapes.size() && notOverlapping; ++j) {
                // ignore the lhs shape as this will be getting removed
                if(j == shapeIndex) {
                    continue;
                }
                if(result[i].aabb.intersects(currentShapes[j].aabb)) {
//cout << "Bounds intersected" << endl;
                    notOverlapping = false;
                }
            }
        }
        return notOverlapping;
    }
    return false;
}


float randf() {
    return rand() / (float)RAND_MAX;
}
*/

int BasicShapeGrammar::selectRule(const Shape& shape, const vector<Rule>& rules)
{
    // selects a rule based on its probability distribution(function)
    // create tmp array of probabilities for each candidate rule
    vector<float> probs(rules.size());
    float total = 0.0f;
    for (int i = 0; i < rules.size(); ++i)
    {
        probs[i] = rules[i].probFunc->getProbability(shape.position);
        total += probs[i];
    }
    if (total == 0.0f)
    {
        return -1;
    }
    float r = randomf() * total;
    int index = 0;
    while (r > probs[index])
    {
        r -= probs[index];
        index++;
    }
    return index;
}
/*
void BasicShapeGrammar::Generate(vector<Shape>& shapes, int maxDepth, SymbolMeshMap& symbolMeshMap) {
    for(int i = 0; i < maxDepth; ++i) {
        /*
cout << "===================" << i << " (" << shapes.size() << ") ============================" << endl;
for(int h = 0; h < shapes.size(); ++h) {
    cout << shapes[h].symbol << " ";
}
cout << endl;


        bool canTerminate = true;
        for(int j = 0; j < shapes.size();) {
            vector<Rule> candidateRules;
            for(int k = 0; k < rules.size(); ++k) {
                if(isCandidateRule(shapes, j, rules[k], symbolMeshMap)) {
                    candidateRules.push_back(rules[k]);
//cout << "candidate rule (" << rules[k].probability << ")" << endl;
                    canTerminate = false;
                }
            }
//cout << candidateRules.size() << " Candidate Rules added" << endl;
            if(candidateRules.size() == 0) {
                ++j;
                continue;
            }
            int index = SelectRule(shapes[j], candidateRules);
            // apply the rule to the shape and add the results to the shapes array
            vector<Shape> result = ApplyRule(shapes[j], candidateRules[index], symbolMeshMap);
//cout << "selected rule: " << index << endl;
            // remove the lhs of the rule from the shapes array
            shapes.erase(shapes.begin()+j);

            // and replace with the rhs
            shapes.insert(shapes.begin()+j, result.begin(), result.end());
            j += result.size();
        }
//cout << "================================================================" << endl;
//if(canTerminate) {
//	break;
//}
    }
}
*/




////////////////////////////////////////////////////////////////////////////////////////////////////


bool BasicShapeGrammar::isCandidateRule(const ShapeDatabase& shapes, const Shape& shape, const Rule& rule)
{
    // if the shape symbol matches the lhs of the rule and is not a terminal shape
    if (shape.symbol == rule.lhsSymbol && !shape.isTerminal())
    {
        // apply the rule to get the list of resultant (rhs) shapes
        std::vector<Shape> result = applyRule(shape, rule, shapes.getSymbolMeshMap());
        // check if any of the new shapes bounding boxes intersect with the current
        // array of shapes (ignoring shapes[shapeIndex])
        for (int i = 0; i < result.size(); ++i)
        {
            // perform an occlusion test for the rhs shape
            if (shapes.occlusionTest(result[i].aabb, &shape))
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

void BasicShapeGrammar::generate(ShapeDatabase& shapes, int numIterations)
{
    for (int i = 0; i < numIterations; ++i)
    {
        for (ShapeDatabase::iterator j = shapes.begin(); j != shapes.end();)
        {
            // because 'isCandidateRule' indirectly calls 'selectRHS' (which is stochastic) we need
            // to save the seeds for each rule so we can replicate the exact produced shape later on
            std::vector<int> seeds;
            // find all candidate rules for the current shape[j]
            vector<Rule> candidateRules;
            for (int k = 0; k < rules.size(); ++k)
            {
                int seed = rand();
                srand(seed);
                if (isCandidateRule(shapes, (*j), rules[k]))
                {
                    candidateRules.push_back(rules[k]);
                    seeds.push_back(seed);
                }
            }
            if (candidateRules.size() == 0)
            {
                ++j;
                continue;
            }
            cout << "applying rule" << endl;
            // out of all the candidate rules - select a random one based
            // on probability
            int index = selectRule(*j, candidateRules);
            if (index == -1) // 0 probability
            {
                ++j;
                continue;
            }
            srand(seeds[index]);
            // apply the rule to the shape and add the results to the shapes array
            vector<Shape> result = applyRule(*j, candidateRules[index], shapes.getSymbolMeshMap());
            // remove the lhs of the rule from the shapes database
            j = shapes.remove(j);
            // insert rhs shapes into database
            for (int k = 0; k < result.size(); ++k)
            {
                j = shapes.insert(j, result[k]);
            }
            j++;
            // and replace with the rhs
            //shapes.insert(shapes.begin()+j, result.begin(), result.end());
          //  for(int k = 0; k < result.size(); ++k) {
            //   shapes.insert(j+k, result[k]);
            //}
           // j += result.size();
        }
        //cout << "================================================================" << endl;
        //if(canTerminate) {
        //	break;
        //}
    }
    //cout << "NUM_SHAPES: " << shapes.getShapeCount() << endl;
}

