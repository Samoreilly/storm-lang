#include <memory>
#include "../parser/condition.h"

/*
    Narrowing conversion
*/

class Conversion {

public:

    std::unique_ptr<DoubleCondition> int_to_double(std::unique_ptr<Condition> in);

    std::unique_ptr<IntegerCondition> double_to_int(std::unique_ptr<Condition> dbl);
};
