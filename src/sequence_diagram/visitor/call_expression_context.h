/**
 * @file src/sequence_diagram/visitor/call_expression_context.h
 *
 * Copyright (c) 2021-2023 Bartek Kryza <bkryza@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include "common/clang_utils.h"
#include "util/util.h"

#include <clang/AST/Expr.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Basic/SourceManager.h>

#include <stack>

namespace clanguml::sequence_diagram::visitor {

struct call_expression_context {
    call_expression_context();

    void reset();

    void dump();

    bool valid() const;

    clang::ASTContext *get_ast_context() const;

    void update(clang::CXXRecordDecl *cls);

    void update(clang::ClassTemplateSpecializationDecl *clst);

    void update(clang::ClassTemplateDecl *clst);

    void update(clang::CXXMethodDecl *method);

    void update(clang::FunctionDecl *function);

    void update(clang::FunctionTemplateDecl *function_template);

    std::int64_t caller_id() const;

    std::int64_t lambda_caller_id() const;

    void set_caller_id(std::int64_t id);

    void enter_lambda_expression(std::int64_t id);

    void leave_lambda_expression();

    clang::IfStmt *current_ifstmt() const;

    void enter_ifstmt(clang::IfStmt *stmt);
    void leave_ifstmt();

    void enter_elseifstmt(clang::IfStmt *stmt);
    clang::IfStmt *current_elseifstmt() const;

    clang::Stmt *current_loopstmt() const;
    void enter_loopstmt(clang::Stmt *stmt);
    void leave_loopstmt();

    clang::Stmt *current_trystmt() const;
    void enter_trystmt(clang::Stmt *stmt);
    void leave_trystmt();

    clang::SwitchStmt *current_switchstmt() const;
    void enter_switchstmt(clang::SwitchStmt *stmt);
    void leave_switchstmt();

    clang::ConditionalOperator *current_conditionaloperator() const;
    void enter_conditionaloperator(clang::ConditionalOperator *stmt);
    void leave_conditionaloperator();

    clang::CallExpr *current_callexpr() const;
    void enter_callexpr(clang::CallExpr *expr);
    void leave_callexpr();

    bool is_expr_in_current_control_statement_condition(
        const clang::Stmt *stmt) const;

    clang::CXXRecordDecl *current_class_decl_{nullptr};
    clang::ClassTemplateDecl *current_class_template_decl_{nullptr};
    clang::ClassTemplateSpecializationDecl
        *current_class_template_specialization_decl_{nullptr};
    clang::CXXMethodDecl *current_method_decl_{nullptr};
    clang::FunctionDecl *current_function_decl_{nullptr};
    clang::FunctionTemplateDecl *current_function_template_decl_{nullptr};

private:
    std::int64_t current_caller_id_{0};
    std::stack<std::int64_t> current_lambda_caller_id_;

    std::stack<clang::CallExpr *> call_expr_stack_;

    std::stack<clang::IfStmt *> if_stmt_stack_;
    std::stack<clang::IfStmt *> elseif_stmt_stack_;

    std::stack<clang::Stmt *> loop_stmt_stack_;
    std::stack<clang::Stmt *> try_stmt_stack_;
    std::stack<clang::SwitchStmt *> switch_stmt_stack_;
    std::stack<clang::ConditionalOperator *> conditional_operator_stack_;
};

} // namespace clanguml::sequence_diagram::visitor