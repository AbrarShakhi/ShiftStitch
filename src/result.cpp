#include "shiftstitch/result.hpp"


namespace shiftstitch {

// ---------------------------------------------------------------------------
// StitchError
// ---------------------------------------------------------------------------
StitchError::StitchError(ErrorCode c, std::string msg) : code(c), message(std::move(msg)) {}

std::string StitchError::toString() const {
	return "[StitchError] " + message;
}

// ---------------------------------------------------------------------------
// Result<void>
// ---------------------------------------------------------------------------
Result<void> Result<void>::Ok() {
	return Result(true);
}

Result<void> Result<void>::Err(StitchError error) {
	return Result(std::move(error));
}

bool Result<void>::isOk() const {
	return ok_;
}

bool Result<void>::isErr() const {
	return !ok_;
}

void Result<void>::value() const {
	if (isErr())
		throw std::runtime_error(error_.value().toString());
}

StitchError& Result<void>::error() {
	if (ok_)
		throw std::logic_error("Result is Ok — no error");
	return error_.value();
}

const StitchError& Result<void>::error() const {
	if (ok_)
		throw std::logic_error("Result is Ok — no error");
	return error_.value();
}

Result<void>::Result(bool ok) : ok_(ok) {}

Result<void>::Result(StitchError e) : ok_(false), error_(std::move(e)) {}

}  // namespace shiftstitch