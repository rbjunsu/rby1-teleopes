/**
 * @file Solver.hpp
 * @author Giulio Romualdi
 * @copyright  Released under the terms of the BSD 3-Clause License
 * @date 2018
 */
#ifndef OSQPEIGEN_SOLVER_HPP
#define OSQPEIGEN_SOLVER_HPP

// Std
#include <memory>

// Eigen
#include <Eigen/Dense>

// OSQP
#include <osqp.h>

// OsqpEigen
#include <OsqpEigen/Compat.hpp>
#include <OsqpEigen/Constants.hpp>
#include <OsqpEigen/Data.hpp>
#include <OsqpEigen/Settings.hpp>

/**
 * OsqpEigen namespace.
 */
namespace OsqpEigen
{
/**
 * Solver class is a wrapper of the OSQP OSQPWorkspace struct.
 */
class Solver
{
    bool m_isSolverInitialized; /**< Boolean true if solver is initialized. */
#ifdef OSQP_EIGEN_OSQP_IS_V1
    std::unique_ptr<OSQPSolver, std::function<void(OSQPSolver*)>> m_solver; /**< Pointer to
                                                                               OSQPSolver struct. */
#else
    std::unique_ptr<OSQPWorkspace, std::function<void(OSQPWorkspace*)>> m_workspace; /**< Pointer to
                                                                                        OSQPWorkspace
                                                                                        struct. */
#endif
    std::unique_ptr<OsqpEigen::Settings> m_settings; /**< Pointer to Settings class. */
    std::unique_ptr<OsqpEigen::Data> m_data; /**< Pointer to Data class. */
    Eigen::Matrix<c_float, Eigen::Dynamic, 1> m_primalVariables;
    Eigen::Matrix<c_float, Eigen::Dynamic, 1> m_dualVariables;
    Eigen::Matrix<c_float, -1, 1> m_solution;
    Eigen::Matrix<c_float, -1, 1> m_dualSolution;

    std::vector<c_int> m_hessianNewIndices;
    std::vector<c_float> m_hessianNewValues;

    std::vector<c_int> m_constraintsNewIndices;
    std::vector<c_float> m_constraintsNewValues;

    std::vector<Eigen::Triplet<c_float>> m_oldHessianTriplet, m_newHessianTriplet,
        m_newUpperTriangularHessianTriplets;
    std::vector<Eigen::Triplet<c_float>> m_oldLinearConstraintsTriplet,
        m_newLinearConstraintsTriplet;

    /**
     * Evaluate the position and the values of the new elements of a sparse matrix.
     * @param oldMatrixTriplet vector containing the triplets of the old sparse matrix;
     * @param newMatrixTriplet vector containing the triplets of the mew sparse matrix;
     * @param newIndices vector of the index mapping new elements
     * to position in the sparse matrix;
     * @param newValues vector of new elements in the sparse matrix.
     * @return true if the sparsity pattern is not changed false otherwise.
     */
    template <typename T>
    bool evaluateNewValues(const std::vector<Eigen::Triplet<T>>& oldMatrixTriplet,
                           const std::vector<Eigen::Triplet<T>>& newMatrixTriplet,
                           std::vector<c_int>& newIndices,
                           std::vector<c_float>& newValues) const;

    /**
     * Takes only the triplets which belongs to the upper triangular part of the matrix.
     * @param fullMatrixTriplets vector containing the triplets of the sparse matrix;
     * @param upperTriangularMatrixTriplets vector containing the triplets of the mew sparse matrix;
     */
    template <typename T>
    void selectUpperTriangularTriplets(
        const std::vector<Eigen::Triplet<T>>& fullMatrixTriplets,
        std::vector<Eigen::Triplet<T>>& upperTriangularMatrixTriplets) const;

#ifdef OSQP_EIGEN_OSQP_IS_V1
    /**
     * Custom Deleter for the OSQPSolver. It is required to free the @ref m_workspace unique_ptr
     * @param ptr raw pointer to the workspace
     */
    static void OSQPSolverDeleter(OSQPSolver* ptr) noexcept;
#else
    /**
     * Custom Deleter for the OSQPWorkspace. It is required to free the @ref m_workspace unique_ptr
     * @param ptr raw pointer to the workspace
     */
    static void OSQPWorkspaceDeleter(OSQPWorkspace* ptr) noexcept;
#endif

    inline const OSQPData* getData() const noexcept
    {
#ifdef OSQP_EIGEN_OSQP_IS_V1
        return m_data->getData();
#else
        return m_workspace->data;
#endif
    }

    inline const OSQPInfo* getInfo() const noexcept
    {
#ifdef OSQP_EIGEN_OSQP_IS_V1
        return m_solver->info;
#else
        return m_workspace->info;
#endif
    }

    inline const OSQPSolution* getOSQPSolution() const noexcept
    {
#ifdef OSQP_EIGEN_OSQP_IS_V1
        return m_solver->solution;
#else
        return m_workspace->solution;
#endif
    }

public:
    /**
     * Constructor.
     */
    Solver();

    /**
     * Initialize the solver with the actual initial data and settings.
     * @return true/false in case of success/failure.
     */
    bool initSolver();

    /**
     * Check if the solver is initialized.
     * @return true if the solver is initialized.
     */
    bool isInitialized();

    /**
     * Deallocate memory.
     */
    void clearSolver();

    /**
     * Set to zero all the solver variables.
     * @return true/false in case of success/failure.
     */
    bool clearSolverVariables();

    /**
     * Solve the QP optimization problem.
     * @return true/false in case of success/failure.
     */
    [[deprecated("Use solveProblem() instead.")]] bool solve();

    /**
     * Solve the QP optimization problem.
     * @return the error exit flag
     */
    OsqpEigen::ErrorExitFlag solveProblem();

    /**
     * Get the status of the solver
     * @return The inner solver status
     */
    OsqpEigen::Status getStatus() const;

    /**
     * Get the primal objective value
     * @return The primal objective value
     */
    c_float getObjValue() const;

    /**
     * Get the optimization problem solution.
     * @return an Eigen::Vector containing the optimization result.
     */
    const Eigen::Matrix<c_float, -1, 1>& getSolution();

    /**
     * Get the dual optimization problem solution.
     * @return an Eigen::Vector containing the optimization result.
     */
    const Eigen::Matrix<c_float, -1, 1>& getDualSolution();

    /**
     * Update the linear part of the cost function (Gradient).
     * @param gradient is the Gradient vector.
     * @note the elements of the gradient are not copied inside the library.
     * The user has to guarantee that the lifetime of the objects passed is the same of the
     * OsqpEigen object.
     * @return true/false in case of success/failure.
     */
    bool
    updateGradient(const Eigen::Ref<const Eigen::Matrix<c_float, Eigen::Dynamic, 1>>& gradient);

    /**
     * Update the lower bounds limit (size m).
     * @param lowerBound is the lower bound constraint vector.
     * @note the elements of the lowerBound are not copied inside the library.
     * The user has to guarantee that the lifetime of the object passed is the same of the
     * OsqpEigen object.
     * @return true/false in case of success/failure.
     */
    bool
    updateLowerBound(const Eigen::Ref<const Eigen::Matrix<c_float, Eigen::Dynamic, 1>>& lowerBound);

    /**
     * Update the upper bounds limit (size m).
     * @param upperBound is the upper bound constraint vector.
     * @note the elements of the upperBound are not copied inside the library.
     * The user has to guarantee that the lifetime of the object passed is the same of the
     * OsqpEigen object.
     * @return true/false in case of success/failure.
     */
    bool
    updateUpperBound(const Eigen::Ref<const Eigen::Matrix<c_float, Eigen::Dynamic, 1>>& upperBound);

    /**
     * Update both upper and lower bounds (size m).
     * @param lowerBound is the lower bound constraint vector;
     * @param upperBound is the upper bound constraint vector.
     * @note the elements of the lowerBound and upperBound are not copied inside the library.
     * The user has to guarantee that the lifetime of the objects passed is the same of the
     * OsqpEigen object
     * @return true/false in case of success/failure.
     */
    bool
    updateBounds(const Eigen::Ref<const Eigen::Matrix<c_float, Eigen::Dynamic, 1>>& lowerBound,
                 const Eigen::Ref<const Eigen::Matrix<c_float, Eigen::Dynamic, 1>>& upperBound);

    /**
     * Update the quadratic part of the cost function (Hessian).
     * It is assumed to be a symmetric matrix.
     * \note
     * If the sparsity pattern is preserved the matrix is simply update
     * otherwise the entire solver will be reinitialized. In this case
     * the primal and dual variable are copied in the new workspace.
     *
     * @param hessian is the Hessian matrix.
     * @return true/false in case of success/failure.
     */
    template <typename Derived>
    bool updateHessianMatrix(const Eigen::SparseCompressedBase<Derived>& hessianMatrix);

    /**
     * Update the linear constraints matrix (A)
     * \note
     * If the sparsity pattern is preserved the matrix is simply update
     * otherwise the entire solver will be reinitialized. In this case
     * the primal and dual variable are copied in the new workspace.
     *
     * @param linearConstraintsMatrix is the linear constraint matrix A
     * @return true/false in case of success/failure.
     */
    template <typename Derived>
    bool updateLinearConstraintsMatrix(
        const Eigen::SparseCompressedBase<Derived>& linearConstraintsMatrix);

    /**
     * Set the entire
     * @param linearConstraintsMatrix is the linear constraint matrix A
     * @return true/false in case of success/failure.
     */
    template <typename T, int n, int m>
    bool setWarmStart(const Eigen::Matrix<T, n, 1>& primalVariable,
                      const Eigen::Matrix<T, m, 1>& dualVariable);

    template <typename T, int n>
    bool setPrimalVariable(const Eigen::Matrix<T, n, 1>& primalVariable);

    template <typename T, int m> bool setDualVariable(const Eigen::Matrix<T, m, 1>& dualVariable);

    template <typename T, int n> bool getPrimalVariable(Eigen::Matrix<T, n, 1>& primalVariable);

    template <typename T, int m> bool getDualVariable(Eigen::Matrix<T, m, 1>& dualVariable);

    /**
     * Get the solver settings pointer.
     * @return the pointer to Settings object.
     */
    const std::unique_ptr<OsqpEigen::Settings>& settings() const;

    /**
     * Get the pointer to the solver initial data.
     * @return the pointer to Data object.
     */
    const std::unique_ptr<OsqpEigen::Data>& data() const;

#ifdef OSQP_EIGEN_OSQP_IS_V1
    /**
     * Get the pointer to the OSQP solver.
     * @return the pointer to Solver object.
     */
    const std::unique_ptr<OSQPSolver, std::function<void(OSQPSolver*)>>& solver() const;
#else
    /**
     * Get the pointer to the OSQP workspace.
     * @return the pointer to Workspace object.
     */
    const std::unique_ptr<OSQPWorkspace, std::function<void(OSQPWorkspace*)>>& workspace() const;
#endif
};

#include <OsqpEigen/Solver.tpp>
} // namespace OsqpEigen

#endif
