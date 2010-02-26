/*
 * Copyright (C) 2008 Herv� Poussineau
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#pragma once

#include "mingw.h"

#include <map>

class Rule
{
public:
	Rule ( const std::string& command, const char *generatedFile1, ... );
	void Execute ( FILE *outputFile,
	               MingwBackend *backend,
	               const Module& module,
	               const FileLocation *source,
	               string_list& clean_files,
	               const std::string& additional_dependencies = "",
	               const std::string& compiler_flags = "" ) const;
	void Execute ( FILE *outputFile,
	               MingwBackend *backend,
	               const Module& module,
	               const FileLocation *source,
	               string_list& clean_files,
	               const std::string& additional_dependencies,
	               const std::string& compiler_flags,
				   const std::map<std::string, std::string>& custom_variables ) const;
private:
	const std::string command;
	string_list generatedFiles;
};
