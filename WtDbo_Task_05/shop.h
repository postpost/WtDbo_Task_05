#pragma once
#include <wt/dbo/dbo.h>
#include <wt/Dbo/backend/Postgres.h>

class Stock;

class Shop {
public:
	std::string name;
	Wt::Dbo::collection<Wt::Dbo::ptr<Stock>> stock;

	template <typename Action>
	void persist(Action& a) {
		Wt::Dbo::field(a, name, "name");
		Wt::Dbo::hasMany(a, stock, Wt::Dbo::ManyToOne, "shop");
	}
};
