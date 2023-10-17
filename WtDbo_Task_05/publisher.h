#pragma once
#include <iostream>
#include <wt/dbo/dbo.h>
#include <wt/Dbo/backend/Postgres.h>
#include "book.h"

class Publisher {
public:
	std::string name;
	Wt::Dbo::collection<Wt::Dbo::ptr<Book>> books;

	template <typename Action>
	void persist(Action& a) {
		Wt::Dbo::field(a, name, "name");
		Wt::Dbo::hasMany(a, books, Wt::Dbo::ManyToOne, "publisher");
	}

	
};
