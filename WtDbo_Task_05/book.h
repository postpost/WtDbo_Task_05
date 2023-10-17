#pragma once
#include <wt/dbo/dbo.h>
#include <wt/Dbo/backend/Postgres.h>
#include "publisher.h"

class Publisher;
class Stock;

class Book {
public:
	std::string title;
	Wt::Dbo::ptr<Publisher> publisher;
	Wt::Dbo::collection<Wt::Dbo::ptr<Stock>> stock;

	template <typename Action>
	void persist(Action& a) {
		Wt::Dbo::field(a, title, "title");
		Wt::Dbo::belongsTo(a, publisher, "publisher");
		Wt::Dbo::hasMany(a, stock, Wt::Dbo::ManyToOne, "book");
	}
};
