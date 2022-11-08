//
// Created by Kirill Zhukov on 08.11.2022.
//

#ifndef UNIT_RPCFILTERBUILDER_H
#define UNIT_RPCFILTERBUILDER_H
#include "iostream"
#include "RpcFilterObject.h"

class RpcFilterBuilder {
public:
    RpcFilterBuilder() = default;

    RpcFilterBuilder(const vector<RpcFilterObject> &filters, const shared_ptr<boost::json::value> &parameter);

    RpcFilterBuilder *setParameter(const std::shared_ptr<boost::json::value> &parameter);
    RpcFilterBuilder *setFilter(const std::shared_ptr<RpcFilterObject>& rpcFilter);
    RpcFilterBuilder *setMultipleFilters(const std::shared_ptr<std::vector<RpcFilterObject>>& filtersList);
    std::tuple<bool, std::shared_ptr<boost::json::value>> build();
private:
    std::vector<RpcFilterObject> filters;
    std::shared_ptr<boost::json::value> parameter;
};

RpcFilterBuilder *RpcFilterBuilder::setFilter(const std::shared_ptr<RpcFilterObject>& rpcFilter) {
    this->filters.emplace_back(*rpcFilter);
    return this;
}

RpcFilterBuilder *RpcFilterBuilder::setMultipleFilters(const std::shared_ptr<std::vector<RpcFilterObject>>& filtersList) {
    this->filters.insert(
            this->filters.end(),
            std::make_move_iterator(filtersList->begin()),
            std::make_move_iterator(filtersList->end())
    );
    #if 0
        std::copy(
          dest.begin(),
          dest.end(),
          std::ostream_iterator<int>(std::cout, "\n")
        );
    #endif
    return this;
}

RpcFilterBuilder *RpcFilterBuilder::setParameter(const shared_ptr<boost::json::value> &parameter) {
    this->parameter = parameter;
    return this;
}

std::tuple<bool, std::shared_ptr<boost::json::value>> RpcFilterBuilder::build() {
    bool filteringResult = true;
    for (auto & filter : this->filters) {
        std::tuple<bool, std::shared_ptr<boost::json::value>> filterResponse = filter.doReturnableFilterInternal(
                this->parameter);
        filteringResult &= std::get<0>(filterResponse);
        if (!filteringResult) return {false, std::get<1>(filterResponse)};
    }
    return {filteringResult, nullptr};
}

RpcFilterBuilder::RpcFilterBuilder(const vector<RpcFilterObject> &filters,
                                   const shared_ptr<boost::json::value> &parameter) : filters(filters),
                                                                                      parameter(parameter) {}

#endif //UNIT_RPCFILTERBUILDER_H
