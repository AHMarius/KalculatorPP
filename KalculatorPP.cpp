#define _USE_MATH_DEFINES

#include <iostream>
#include <cstring>
#include <stack>
#include <string>
#include <cmath>
#include <iomanip>

const int MAX_LEN = 200;
const double e = M_E;
const double p = M_PI;

// Math Engine 1.0
bool containsOnlyDigits(const std::string& str) {
    for (char c : str) {
        if (!isdigit(c) && c != '.') {
            return false;
        }
    }
    return !str.empty();
}

bool performNmb(double& a, const std::string& input) {
    if (containsOnlyDigits(input)) {
        a = std::stod(input);
    }
    else if (input == "e") {
        a = e;
    }
    else if (input == "p") {
        a = p;
    }
    else {
        return false;
    }
    return true;
}

bool performOp(double& a, double b, const std::string& op) {
    if (op == "+") {
        a += b;
    }
    else if (op == "-") {
        a -= b;
    }
    else if (op == "*") {
        a *= b;
    }
    else if (op == "/") {
        if (b == 0) {
            std::cerr << "Error: Division by zero" << std::endl;
            return false;
        }
        a /= b;
    }
    else {
        std::cerr << "Error: Unknown operator " << op << std::endl;
        return false;
    }

    return true;
}

void performCalc(std::stack<double>& calcNmbTxt, std::stack<std::string>& calcOpTxt) {
    if (calcNmbTxt.size() < 2 || calcOpTxt.empty()) {
        std::cerr << "Error: Insufficient operands or empty operator stack" << std::endl;
        return;
    }

    double nmb1, nmb2;
    nmb2 = calcNmbTxt.top();
    calcNmbTxt.pop();
    nmb1 = calcNmbTxt.top();
    calcNmbTxt.pop();

    std::string op = calcOpTxt.top();
    calcOpTxt.pop();

    if (!performOp(nmb1, nmb2, op)) {
        std::cerr << "Error: Failed to perform operation" << std::endl;
        return;
    }

    calcNmbTxt.push(nmb1);
}

double funcCalc(double x, const std::string& functionType) {
    const double EPSILON = 1e-12; // Adjusted epsilon

    if (functionType == "sin") {
        if (std::abs(std::fmod(x, M_PI)) < EPSILON || std::abs(std::fmod(x, M_PI) - M_PI) < EPSILON) {
            return 0.0;
        }
        return std::sin(x);
    }
    else if (functionType == "cos") {
        if (std::abs(std::fmod(x, M_PI / 2.0)) < EPSILON || std::abs(std::fmod(x, M_PI / 2.0) - (M_PI / 2.0)) < EPSILON) {
            return 0.0;
        }
        return std::cos(x);
    }
    else if (functionType == "tan") {
        if (std::abs(std::fmod(x, M_PI)) < EPSILON) {
            return 0.0;
        }
        return std::tan(x);
    }
    else if (functionType == "ctg") {
        if (std::abs(std::fmod(x, M_PI)) < EPSILON || std::abs(std::fmod(x, M_PI) - M_PI / 2.0) < EPSILON) {
            return 0.0;
        }
        return 1.0 / std::tan(x);
    }
    else if (functionType == "ln") {
        if (x <= 0) {
            std::cerr << "Error: Invalid argument for ln" << std::endl;
            return 0.0; // Handle invalid argument gracefully
        }
        return std::log(x);
    }
    return 0;
}

double evaluateExpression(std::stack<double>& calcNmbTxt, std::stack<std::string>& calcOpTxt) {
    while (!calcOpTxt.empty()) {
        performCalc(calcNmbTxt, calcOpTxt);
    }
    double result = calcNmbTxt.top();
    calcNmbTxt.pop();
    return result;
}

double parseFunction(char*& p, char*& nextToken, std::stack<double>& calcNmbTxt, std::stack<std::string>& calcOpTxt) {
    std::string funcName = p;
    funcName.pop_back(); // Remove '('
    p = strtok_s(NULL, " ", &nextToken);

    std::stack<double> funcNmbStack;
    std::stack<std::string> funcOpStack;

    while (p && strcmp(p, ")") != 0) {
        if (std::strchr("+-*/", p[0])) {
            funcOpStack.push(std::string(p));
        }
        else {
            double nmb;
            std::string token(p);
            if (token.find("sin(") == 0 || token.find("cos(") == 0 || token.find("tan(") == 0 || token.find("ctg(") == 0 || token.find("ln(") == 0) {
                nmb = parseFunction(p, nextToken, calcNmbTxt, calcOpTxt);
            }
            else if (!performNmb(nmb, token)) {
                std::cerr << "Error: Invalid operand in function " << funcName << std::endl;
                return 0;
            }
            funcNmbStack.push(nmb);
        }
        p = strtok_s(NULL, " ", &nextToken);
    }

    if (!p || strcmp(p, ")") != 0) {
        std::cerr << "Error: Unmatched parentheses in function " << funcName << std::endl;
        return 0;
    }

    double argValue = evaluateExpression(funcNmbStack, funcOpStack);
    return funcCalc(argValue, funcName);
}

bool readingCalc(std::stack<double>& calcNmbTxt, std::stack<std::string>& calcOpTxt) {
    char calcPartTxt[MAX_LEN] = "";
    std::cin.getline(calcPartTxt, MAX_LEN);

    char* nextToken;
    char* p = strtok_s(calcPartTxt, " ", &nextToken);
    while (p) {
        if (std::strchr("+-*/", p[0])) {
            calcOpTxt.push(std::string(p));
        }
        else {
            double nmb1;
            std::string token(p);
            if (token.find("sin(") == 0 || token.find("cos(") == 0 || token.find("tan(") == 0 || token.find("ctg(") == 0 || token.find("ln(") == 0) {
                nmb1 = parseFunction(p, nextToken, calcNmbTxt, calcOpTxt);
            }
            else if (!performNmb(nmb1, token)) {
                std::cerr << "Error: Invalid operand at position " << calcNmbTxt.size() + 1 << std::endl;
                return false;
            }
            calcNmbTxt.push(nmb1);
        }
        p = strtok_s(NULL, " ", &nextToken);
    }

    return calcNmbTxt.size() == calcOpTxt.size() + 1;
}

int main() {
    std::stack<double> calcNmbTxt;
    std::stack<std::string> calcOpTxt;
    std::cout << "This calculator is just a fun way to learn a few concepts about the RPN and it has a lot of bugs, p stands for pi and e stands for the Euler's number. You can also use the next functions (sin,cos,tan,ctg,ln)" << std::endl;
    if (!readingCalc(calcNmbTxt, calcOpTxt)) {
        std::cerr << "Error: Invalid number of operands" << std::endl;
        return 1;
    }

    while (calcNmbTxt.size() > 1) {
        performCalc(calcNmbTxt, calcOpTxt);
    }

    std::cout << std::setprecision(15) << calcNmbTxt.top() << std::endl;

    return 0;
}