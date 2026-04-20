#pragma once

#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>

namespace shiftstitch {

enum class ErrorCode {
	ImageLoadFailed,
	NoImagesProvided,
	FeatureSelectionFailed,
	EmptyDescriptors,
	NotEnoughMatches,
	HomographyFailed,
	WarpFailed,
	PanoramaNotCreated,
	SaveFailed,
};

struct StitchError {
	ErrorCode code;
	std::string message;

	StitchError(ErrorCode c, std::string msg);

	[[nodiscard]] std::string toString() const;
};

// ---------------------------------------------------------------------------
// Result<T>
// ---------------------------------------------------------------------------
template <typename T>
class Result {
public:
	static Result Ok(T value) {
		return Result(std::move(value));
	}

	static Result Err(StitchError error) {
		return Result(std::move(error));
	}

	[[nodiscard]] bool isOk() const {
		return std::holds_alternative<T>(data_);
	}

	[[nodiscard]] bool isErr() const {
		return !isOk();
	}

	[[nodiscard]] T& value() {
		if (isErr())
			throw std::runtime_error(error().toString());
		return std::get<T>(data_);
	}

	[[nodiscard]] const T& value() const {
		if (isErr())
			throw std::runtime_error(error().toString());
		return std::get<T>(data_);
	}

	[[nodiscard]] StitchError& error() {
		return std::get<StitchError>(data_);
	}

	[[nodiscard]] const StitchError& error() const {
		return std::get<StitchError>(data_);
	}

	[[nodiscard]] T valueOr(T fallback) const {
		return isOk() ? std::get<T>(data_) : std::move(fallback);
	}

	template <typename F>
	auto map(F&& f) -> Result<decltype(f(std::declval<T>()))> {
		using U = decltype(f(std::declval<T>()));
		if (isErr())
			return Result<U>::Err(error());
		return Result<U>::Ok(f(value()));
	}

private:
	std::variant<T, StitchError> data_;

	explicit Result(T v) : data_(std::move(v)) {}
	explicit Result(StitchError e) : data_(std::move(e)) {}
};

// ---------------------------------------------------------------------------
// Result<void>
// ---------------------------------------------------------------------------
template <>
class Result<void> {
public:
	static Result Ok();
	static Result Err(StitchError error);

	[[nodiscard]] bool isOk() const;
	[[nodiscard]] bool isErr() const;

	void value() const;

	[[nodiscard]] StitchError& error();
	[[nodiscard]] const StitchError& error() const;

private:
	bool ok_;
	std::optional<StitchError> error_;

	explicit Result(bool ok);
	explicit Result(StitchError e);
};

}  // namespace shiftstitch