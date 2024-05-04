#ifndef TRANSFORMER_H
#define TRANSFORMER_H
#include <memory>
#include "global.h"

class Transformer {
public:
    /**
     * Constructs a Transformer object which takes the following transformation object
     * as an optional parameter
     * @param next [IN] shared pointer to next step in transformation pipeline.
     *  Defaults to a nullptr.
     */
    Transformer(std::shared_ptr<Transformer> next = std::shared_ptr<Transformer>());

    /**
     * Destructor
     */
    virtual ~Transformer();

    /**
     * Applies the transformation implemented by this object and
     * all chained transformation objects to the given input.
     * @param input
     * @return input that has been processed through transformation chain
     */
    uint32_t applyTransformations(uint32_t input);

protected:
    /**
     * Applies the implemented transformation to the given input
     * and returns the output. Transformations can be a filter,
     * scaling etc.
     * @param input
     * @return Transformed input
     */
    virtual uint32_t transform(uint32_t input) = 0;

    /**
     * Next step in transformation pipeline
     */
    const std::shared_ptr<Transformer> m_next;
};


#endif //TRANSFORMER_H
