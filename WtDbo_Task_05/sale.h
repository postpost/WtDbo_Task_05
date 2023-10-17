#pragma once
#include <wt/dbo/dbo.h>
#include <wt/Dbo/backend/Postgres.h>

class Stock;

class Sale {
public:
	int price;
	std::string date_sale;
	int count;
	//id_stock
	Wt::Dbo::ptr<Stock> stock;

	template<typename Action>
	void persist(Action& a) {
		Wt::Dbo::field(a, price, "price");
		Wt::Dbo::field(a, date_sale, "date_sale");
		Wt::Dbo::field(a, count, "count");
		Wt::Dbo::belongsTo(a, stock, "stock");
	}
};
