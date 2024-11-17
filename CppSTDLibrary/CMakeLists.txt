cmake_minimum_required(VERSION 3.16)
project(CppSTDLibrary)

set(CMAKE_CXX_STANDARD 26)
# add_compile_options(-c -Wall -Wextra -O3 -std=c++26)
add_compile_options(-c -Wall -O3 -std=c++26)

add_executable(${PROJECT_NAME}
        main.cpp
		Types.cpp
		AggregateInitialization/AggregateInitialization.cpp
		Algorithms/Algorithms.cpp
		Any/Any.cpp
		Arrays/Arrays.cpp
		Asserts/Asserts.cpp
		Atomic/Atomic.cpp
		Attributes/Attributes.cpp
		Auto/Auto.cpp
		BitSet/BitSet.cpp
		BitwiseOperation/BitwiseOperation.cpp
		Byte/Byte.cpp
		Chrono/Chrono.cpp
		Chrono/TimeZones.cpp
		Concepts/Concepts.cpp
		CTAD/ClassTemplateArgumentDeduction.cpp
		ConsoleInOut/ConsoleInOut.cpp
		DateAndTime/DateAndTime.cpp
		ConstConstexprMutable/ConstConstexprMutable.cpp
		ConstConstexprMutable/CompileTimeProgramming.cpp
		EnumTests/EnumTests.cpp
		Exceptions/Exceptions.cpp
		Errors/Errors.cpp
		Helpers/UnorderedPair.h
		FilesStreams/FilesStreams.cpp
		Filesystem/Filesystem.cpp
		Format/Format.cpp
		Format/CustomTypesFormatters.cpp
		FunctionObjects/FunctionObjects.cpp
		FunctionObjects/MemberFunctionPointer.cpp
		FunctionObjects/CallbackTests.cpp
		Initialization/Initialization.cpp
		InitializerList/InitializerList.cpp
		InlineVariables/InlineVariables.cpp
		Integer/Integer.cpp
		IteratorTests/IteratorTests.cpp
		Lambdas/Lambdas.cpp
		LifetimeExtension/LifetimeExtension.cpp
		Literals/Literals.cpp
		Locale/Locale.cpp
		Overflow/Overflow.cpp
		Math/Math.cpp
		Memory/Memory.cpp
		Memory/MemoryUsageMonitor.cpp
		Memory/Alignment.cpp
		Memory/AlignedStackAllocator.cpp
		Memory/CustomStackAllocator.cpp
		MoveSemantics_RuleOfFive/MoveSemantics_RuleOfFive.cpp
		Numbers/Numbers.cpp
		NumericLimits/NumericLimits.cpp
		ObjectOrientedProgramming/ObjectOrientedProgramming.cpp
		ObjectOrientedProgramming/Operators.cpp
		ObjectOrientedProgramming/RefQualifierMemberFunctions.cpp
		OptionalTests/OptionalTests.cpp
		PolymorphicMemoryResources/PolymorphicMemoryResources.cpp
		Random/Random.cpp
		Ranges/Ranges.cpp
		ReferenceWrapper/ReferenceWrapper.cpp
		SharedPtr/SharedPtrTests.cpp
		SourceLocation/SourceLocation.cpp
		Span/Span.cpp Span/Span.h
		Print/Print.cpp Print/Print.h
		Streams/Streams.cpp Streams/Streams.h
		String/String.cpp
		String/StringUtilities.cpp
		StringStream/StringStreamTests.cpp
		StringView/StringViewTests.cpp
		StructuredBinding/StructuredBinding.cpp
		StrongTypes/StrongTypes.cpp
		Tuple/TupleTests.cpp
		TypeCast/TypeCast.cpp
		TypeTraits/TypeTraits.cpp
		UniquePtr/UniquePtr.cpp
		Utilities/Utilities.cpp
		Utilities/Bind.cpp
		Utilities/CompilerVersion.cpp
		Variant/Variant.cpp
		WeakPtr/WeakPtr.cpp
        Volatile/VolatileTests.cpp
		Hashing/Hashing.cpp
		Metaprogramming/Metaprogramming.cpp
		StarshipOperator/StarshipOperator.cpp
		Expected/Expected.cpp
		Types/Int.cpp
		NameSpaces/InlineNamespaces.cpp
		Comparators/Comparators.cpp
		Templates/Templates.cpp Templates/Templates.h
		Templates/ConditionalExplicit.cpp
		Templates/FoldExpressions.cpp
		Templates/NTTP.cpp
		Templates/TemplateSpecialization.cpp
		RVO_CopyElision/CopyElision.cpp
		RVO_CopyElision/RVO_FailureCases.cpp
		Overflow/Overflow.cpp
)

# Link libraries to the project/executable
target_link_libraries(${PROJECT_NAME} PUBLIC
		pthread
		${EXTRA_LIBS}
)