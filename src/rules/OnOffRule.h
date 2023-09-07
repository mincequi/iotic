#pragma once

#include <functional>
#include <memory>
#include <rpp/subjects/publish_subject.hpp>
#include <rules/Rule.h>

namespace exprtk {
template<class>
class expression;
}

using ExprPtr = std::unique_ptr<exprtk::expression<double>>;
using Action = std::function<void(bool)>;

class OnOffRule : public Rule {
public:
    OnOffRule(const std::string& thingId, ExprPtr&& onExpression, ExprPtr&& offExpression, Action action);
    ~OnOffRule();

    void evaluate() const override;

private:
    ExprPtr _onExpression;
    ExprPtr _offExpression;
    Action _action;
    rpp::subjects::publish_subject<bool> _subject;
};
