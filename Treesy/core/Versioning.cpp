// Copyright (c) 2025 Josh Sellers
// Licensed under the MIT License. See LICENSE file.

#include "Versioning.h"

const std::string VERSION = "1.1";

const std::string BUILD_NUMBER =
std::to_string(BUILD_DATE_YEAR_INT - 2000)
+ CONSONANTS_MONTH.at(BUILD_DATE_MONTH_INT - 1)
+ LETTERS_DAY.at(BUILD_DATE_DAY_INT - 1)
+ LETTERS_HOUR.at(BUILD_TIME_HOURS_INT)
+ "-" + WORDS_MINUTE.at(BUILD_TIME_MINUTES_INT)
+ "-" + WORDS_MINUTE.at(BUILD_TIME_SECONDS_INT);
