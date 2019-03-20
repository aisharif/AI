def qlearning_update(self, state, action, nextState, reward):
    estimatedQ = reward + self.discount * self.computeValueFromQValues(nextState)
    self.qValues[(state,action)] = estimatedQ
